#ifndef ASPIRE_TERMINAL_PTY
#define ASPIRE_TERMINAL_PTY

#include <QObject>
#include <QSocketNotifier>

#include <bfd.h>
#include <cstddef> // for std::size_t
#include <string_view>
#include <qtmetamacros.h>
#include <sys/types.h>

/**
 * Wrapper class for an instance of a pseudoterminal.
 */
class Pty : public QObject
{
    Q_OBJECT

public:
    /* The maximum amount of bytes that can be revieved from the input buffer at a time. */
    static const std::size_t READ_BUFFER_SIZE = 65536;

    Pty();
    ~Pty();
    /* Creates the pseudoterminal and starts the given process. */
    void start(std::size_t width, std::size_t height, const char* processName=std::getenv("SHELL"));
    /* Kills the pseudoterminal process and disables the Pty. */
    void stop();
    /* Returns true if the Pty is running and false if it is not. */
    const bool isAlive();
    /* Changes the reported character size of the terminal. */
    void resize(std::size_t width, std::size_t height);
    /* Sends the given character(s) into the input of the Pty. */
    int send(const char* sequence, std::size_t length);

    std::string_view prompt();

signals:
    /* Sent when the terminal application gives output. */
    void recieved(std::string_view output);
    
public slots:
    void fdReadNotified(QSocketDescriptor socket, QSocketNotifier::Type type);

private:
    /* Returns all text recieved from the terminal since last call to recieve(). */
    std::string_view recieve();
    /* Initializes a pseudoterminal and sets m_fileDescriptor to the master file descriptor. */
    void openpt();
    /* Sends a size change signal to the master file descriptor. */
    void setSize(std::size_t width, std::size_t height);
    /* Initializes prompt address */
    void getPromptAddr(const char* shellName);
    /* Creates a child process and executes the shell on it. Returns whether or not this is the child process. */
    bool forkpt(const char* shellName);

    QSocketNotifier m_fdReader;
    pid_t m_processId {0};
    int m_fileDescriptor;
    bfd_vma m_promptAddr;
}; 

#endif