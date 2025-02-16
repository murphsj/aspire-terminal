#ifndef ASPIRE_TERMINAL_TERMINAL_WIDGET
#define ASPIRE_TERMINAL_TERMINAL_WIDGET

#include <chrono>
#include <string_view>

#include <QPainter>
#include <QTimer>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "EscapeSequence.h"
#include "Pty.h"
#include "TextBuffer.h"

class TerminalWidget: public QWidget
{
    Q_OBJECT

public:
    constexpr static std::chrono::milliseconds BlinkInterval { 500 };
    TerminalWidget(QWidget* parent = nullptr, TerminalCharacter* characterData = nullptr);

public slots:
    /** Activated when data is written to the terminal. */
    void recievedFdData(std::string_view output);
    void blinkEvent();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void paintBackground(QPainter& painter, QRect& region);
    void paintAllCharacters(QPainter& painter, QRect& region);
    void paintCharacter(QPainter& painter, QRect& region, TerminalCharacter c, bool drawCursorHighlight);
    /**
     * Reads one complete character/control sequence from the string at the given index, parses it, and processess the result.
     * Returns the amount of characters read.
     */
    ssize_t parse(std::string_view output, ssize_t index);
    ssize_t parseEscapeSequence(std::string_view output, ssize_t index);
    ssize_t parseCharacter(std::string_view output, ssize_t index);
    void applyEscapeSequence(EscapeSequence esc);
    void applyControlSequence(EscapeSequence cs);
    QTimer m_blinkTimer;
    bool m_blinkOn;
    QFont m_font;
    QFontMetrics m_fontMetrics;
    QPen m_pen;
    TextBuffer m_buffer;
    Pty m_pty;
};

#endif