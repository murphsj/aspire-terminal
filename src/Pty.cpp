#include "Pty.h"

#include <asm-generic/ioctls.h>
#include <csignal>
#include <cstdlib>
#include <qglobal.h>
#include <sys/ioctl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h> // for kill(), SIGKILL
#include <termios.h>
#include <unistd.h> // for read(), write()
#include <fcntl.h>
#include <qdebug.h>
#include <bfd.h>
#include <sstream>

#include <QSocketNotifier>

// This is required because QOverload doesn't seem to work with private signals
// I don't really understand QT signals that well so there may be a better way to do this
template<class T> auto privateSignalOverload(void (QSocketNotifier::* s)(QSocketDescriptor, QSocketNotifier::Type, T))
{
    return s;
}

Pty::Pty()
    : m_fdReader(QSocketNotifier::Type::Read)
{
    connect(&m_fdReader, privateSignalOverload(&QSocketNotifier::activated), this, &Pty::fdReadNotified);
}

Pty::~Pty()
{
    stop();
}

// Pty forking code based on eduterm:
// https://www.uninformativ.de/git/eduterm/files.html

void Pty::openpt()
{
    // Open a PTY. We get the master FD from this
    m_fileDescriptor = posix_openpt(O_NOCTTY | O_RDWR);

    // Unlock access to the slave FD
    if (grantpt(m_fileDescriptor) == -1) {
        qFatal("grantpt() failed");
    }
    if (unlockpt(m_fileDescriptor) == -1) {
        qFatal("unlockpt() failed");
    }
}

void Pty::setSize(std::size_t width, std::size_t height)
{
    struct winsize size = {
        .ws_row = static_cast<unsigned short>(height),
        .ws_col = static_cast<unsigned short>(width),
    };

    qDebug() << size.ws_col;

    int res {ioctl(m_fileDescriptor, TIOCSWINSZ, &size)};
    if (res == -1) {
        qFatal("ioctl(TIOCWINSZ) failed");
    }
}

void Pty::getPromptAddr(const char* shellName)
{
    // libBFD code based on https://pramode.net/articles/lfy/ptrace/pramode.html
    bfd_init();
    bfd* abfd {bfd_openr(shellName, nullptr)};
    if (abfd == 0) {
        qFatal("bfd_openr() failed");
    }
    bfd_check_format(abfd, bfd_object);
    long size {bfd_get_symtab_upper_bound(abfd)};
    if (size == 0) {
        qFatal("bfd_get_symtab_upper_bound() failed");
    }

    asymbol** symbolTable = new asymbol*[size];
    long symbolCount {bfd_canonicalize_symtab(abfd, symbolTable)};
    if (symbolCount == 0) {
        qFatal("bfd_canonicalize_symtab() failed");
    }

    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(bfd_asymbol_name(symbolTable[i]), "rl_line_buffer") == 0) {
            m_promptAddr = bfd_asymbol_value(symbolTable[i]);
            return;
        }
    }
}

bool Pty::forkpt(const char* shellName)
{
    char* slaveName {ptsname(m_fileDescriptor)};
    int slaveFd {open(slaveName, O_NOCTTY | O_RDWR)};
    if (slaveFd == -1) {
        qFatal("open(m_slaveName) failed");
    }

    // fork() spawns a new process and returns the pid on the parent but 0 on the child,
    pid_t slavePid {fork()};
    if (slavePid == 0) {
        
        // We're in the child process - close the master fd and pipe input to stdin/out/err
        close(m_fileDescriptor);
        setsid();
        if (ioctl(slaveFd, TIOCSCTTY, 0) == -1) {
            qFatal("ioctl() failed");
        }

        dup2(slaveFd, STDIN_FILENO);
        dup2(slaveFd, STDOUT_FILENO);
        dup2(slaveFd, STDERR_FILENO);
        close(slaveFd);

        //ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        // Spawn the shell in interactive mode
        execl(shellName, shellName, "-l", "-i", nullptr);

        //execl("/home/murphsj/aspire/qt-test/aspireTerminalPrototype/ptrace_child_test", "ptrace_child_test", nullptr);

        exit(0);
    } else if (slaveFd >= 0) {
        m_processId = slavePid;
        close(slaveFd);

        return true;
    } else {
        qFatal("fork() failed");
    }
}

std::string readMemoryString(pid_t child, bfd_vma addr)
{
    kill(child, SIGSTOP);
    int status;
    waitpid(child, &status, WUNTRACED | WNOHANG);
    if (!WIFSTOPPED(status)) {
        qDebug() << "Child was not stopped";
        //return "";
    }

    // ptrace can only read 8 bytes of memory at a time, so to read a C string we need to read repeatedly until we find a null terminator
    std::ostringstream stream {};
    std::size_t readPosition {0};
    while (true) {
        const long data {ptrace(PTRACE_PEEKDATA, child, &addr + readPosition)};

        if (data == -1) {
            perror("PTRACE_PEEKDATA");
            break;
        }
        // We convert to char* so that anything after the null terminator doesn't get added to the stream
        stream << reinterpret_cast<const char*>(&data);
        // Stop reading once there's a null terminator
        if (memchr(&data, '\0', sizeof(data))) break;

        readPosition += 4;
    }

    if (ptrace(PTRACE_CONT, child, 1, NULL) == -1) perror("PTRACE_CONT");

    return stream.str();
}

std::string_view Pty::prompt()
{
    return std::string_view{readMemoryString(m_processId, m_promptAddr)};
}

void Pty::start(std::size_t width, std::size_t height, const char* shellName)
{
    openpt();
    if (!forkpt(shellName)) return; 
    setSize(width, height);

    m_fdReader.setSocket(m_fileDescriptor);
    m_fdReader.setEnabled(true);
}

void Pty::stop()
{
    m_fdReader.setEnabled(false);
    kill(m_processId, SIGKILL);
    m_processId = 0;
    m_fileDescriptor = 0;
}

const bool Pty::isAlive()
{
    return m_fileDescriptor != -2;
}

void Pty::resize(std::size_t width, std::size_t height)
{
    // unimplemented
    Q_ASSERT(0);
}

int Pty::send(const char* sequence, std::size_t length)
{
    if (length > 0 && isAlive()) {
        return write(m_fileDescriptor, sequence, length);
    } else {
        return 0;
    }
}

std::string_view Pty::recieve()
{
    char readBuffer[READ_BUFFER_SIZE];
    ssize_t len {read(m_fileDescriptor, readBuffer, READ_BUFFER_SIZE)};
    return std::string_view(readBuffer, len);
}

void Pty::fdReadNotified(QSocketDescriptor, QSocketNotifier::Type)
{
    recieved(recieve());
}