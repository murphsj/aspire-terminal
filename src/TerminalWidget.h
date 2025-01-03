#ifndef ASPIRE_TERMINAL_TERMINAL_WIDGET
#define ASPIRE_TERMINAL_TERMINAL_WIDGET

#include <string_view>

#include <QPainter>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "Pty.h"
#include "TextBuffer.h"

class TerminalWidget: public QWidget
{
    Q_OBJECT

public:
    TerminalWidget(QWidget* parent = nullptr, TerminalCharacter* characterData = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    /* Activated when data is written to the terminal. */
    void recievedFdData(std::string_view output);

private:
    void paintBackground();
    void paintAllCharacters(QPainter& painter, QRect& region);
    void paintCharacter(QPainter& painter, QRect& region, TerminalCharacter c);
    /*
     * Reads one complete character/control sequence from the string at the given index, parses it, and processess the result.
     * Returns the amount of characters read.
     **/
    ssize_t parse(std::string_view output, ssize_t index);
    ssize_t parseEscapeSequence(std::string_view output, ssize_t index);
    ssize_t parseCharacter(std::string_view output, ssize_t index);
    QFont m_font;
    QFontMetrics m_fontMetrics;
    TextBuffer m_buffer;
    Pty m_pty;
};

#endif