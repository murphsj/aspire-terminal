#ifndef ASPIRE_TERMINAL_TERMINAL_WIDGET
#define ASPIRE_TERMINAL_TERMINAL_WIDGET

#include <chrono>
#include <qstringlistmodel.h>
#include <string_view>

#include <QPainter>
#include <QTimer>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "EscapeSequence.h"
#include "ShellCompletionModel.h"
#include "ShellCompleter.h"
#include "Pty.h"
#include "TextBuffer.h"

class TerminalWidget: public QWidget
{
    Q_OBJECT

public:
    constexpr static std::chrono::milliseconds BlinkInterval { 500 };
    TerminalWidget(QWidget* parent = nullptr, std::size_t width=50, std::size_t height=400);

    void updateCompletion();

public slots:
    /** Activated when data is written to the terminal. */
    void recievedFdData(std::string_view output);
    /** Activated when a completion option is selected. */
    void completionActivated(const QModelIndex& completion);
    void blinkEvent();
    void writeResults();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    QSize sizeHint() const override;

private:
    void paintBackground(QPainter& painter, QRect& region);
    void paintAllCharacters(QPainter& painter, QRect& region);
    void paintCharacter(QPainter& painter, QRect& region, TerminalCharacter c, bool drawCursorHighlight);
    QRect getCharRect(std::size_t cols, std::size_t rows);
    /**
     * Reads one complete character/control sequence from the string at the given index, parses it, and processess the result.
     * Returns the amount of characters read.
     */
    ssize_t parse(std::string_view output, ssize_t index);
    ssize_t parseEscapeSequence(std::string_view output, ssize_t index);
    ssize_t parseCharacter(std::string_view output, ssize_t index);
    void applyEscapeSequence(EscapeSequence esc);
    void applyControlSequence(EscapeSequence cs);
    ShellCompletionModel* m_completionModel;
    ShellCompleter* m_completer;
    QTimer m_blinkTimer;
    bool m_blinkOn;
    QFont m_font;
    QFontMetrics m_fontMetrics;
    QPen m_pen;
    TextBuffer m_buffer;
    Pty m_pty;
    int m_completionsUsed;
    std::size_t m_width;
    std::size_t m_height;
};

#endif