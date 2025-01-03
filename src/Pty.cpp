#include "Pty.h"

#include <asm-generic/ioctls.h>
#include <csignal>
#include <cstdlib>
#include <qglobal.h>
#include <qsocketnotifier.h>
#include <sys/ioctl.h>
#include <signal.h> // for kill(), SIGKILL
#include <termios.h>
#include <unistd.h> // for read(), write()
#include <fcntl.h>

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

    int res {ioctl(m_fileDescriptor, TIOCSWINSZ, &size)};
    if (res == -1) {
        qFatal("ioctl(TIOCWINSZ) failed");
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

        // Spawn the shell in interactive mode
        execl(shellName, shellName, "-l", "-i", nullptr);

        return false;
    } else if (slaveFd >= 0) {
        close(slaveFd);

        return true;
    } else {
        qFatal("fork() failed");
    }
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
    ssize_t len = {read(m_fileDescriptor, readBuffer, READ_BUFFER_SIZE)};
    return std::string_view(readBuffer, len);
}

void Pty::fdReadNotified(QSocketDescriptor, QSocketNotifier::Type)
{
    recieved(recieve());
}