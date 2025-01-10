#include "TerminalWidget.h"
#include "EscapeSequence.h"
#include "TerminalCharacter.h"

#include <QPainter>
#include <QKeyEvent>
#include <qwidget.h>

TerminalWidget::TerminalWidget(QWidget *parent, TerminalCharacter* _c)
    : QWidget(parent)
    , m_font("Monospace")
    , m_fontMetrics(m_font)
    , m_buffer(60, 60)
{
    // Style hint should result in a monospace font being found even if Monospace isn't available
    // (as is the case on Windows)
    m_font.setStyleHint(QFont::Monospace);
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_pty, &Pty::recieved, this, &TerminalWidget::recievedFdData);
    m_pty.start(60, 60);
}

void TerminalWidget::paintEvent(QPaintEvent* event)
{
    QRect region { contentsRect() };
    QPainter painter(this);
    painter.setFont(m_font);
    paintAllCharacters(painter, region);
}

void TerminalWidget::keyPressEvent(QKeyEvent* event)
{
    if (!event->text().isEmpty()) {
        const char* inputText { event->text().toUtf8().constData() };
        m_pty.send(inputText, event->text().length());
        update();
    }
}

void TerminalWidget::paintAllCharacters(QPainter& painter, QRect& region)
{
    QPoint topLeft = region.topLeft();
    int charWidth { m_fontMetrics.averageCharWidth() };
    int charHeight { m_fontMetrics.height() };
    int rows { 0 };
    int cols { 0 };
    for (QVector<TerminalCharacter> line : m_buffer) {
        for (TerminalCharacter c : line) {
            cols++;
            if (cols > m_buffer.getColumns()) {
                cols = 0;
                rows++;
            }
            QRect drawRect = QRect(cols * charWidth, rows * charHeight, charWidth, charHeight);
            paintCharacter(painter, drawRect, c);
        }
    }
}

void TerminalWidget::paintCharacter(QPainter& painter, QRect& region, TerminalCharacter c)
{
    painter.drawText(region, c.character);
}

void TerminalWidget::recievedFdData(std::string_view output)
{
    ssize_t i {0};
    while (i < output.length()) {
        ssize_t newIndex {parse(output, i)};
        i = newIndex;
    }

    update();
}

ssize_t TerminalWidget::parse(std::string_view output, ssize_t index)
{
    static const char ESC = '\033';

    const char& ch = output.at(index);
    if (ch == ESC) {
        return parseEscapeSequence(output, ++index);
    } else {
        return parseCharacter(output, index);
    }
}

ssize_t TerminalWidget::parseCharacter(std::string_view seq, ssize_t index)
{
    m_buffer.write(TerminalCharacter{ seq.at(index) });
    return ++index;
}

ssize_t TerminalWidget::parseEscapeSequence(std::string_view seq, ssize_t index)
{
    EscapeSequence escSequence {};
    ssize_t newIndex {escSequence.read(seq, index)};
    
    // If this is a valid escape sequence, carry out what it's asking us to do
    if (!escSequence.isInvalid()) {
        switch (escSequence.getSequenceType()) {
        case EscapeSequence::SequenceType::ESC:
            applyEscapeSequence(escSequence);
            break;
        case EscapeSequence::SequenceType::CSI:
            applyControlSequence(escSequence);
            break;
        case EscapeSequence::SequenceType::OSC:
            break;
        }
    }

    escSequence.debugInfo();

    return newIndex;
}

void TerminalWidget::applyControlSequence(EscapeSequence cs)
{

}

void TerminalWidget::applyEscapeSequence(EscapeSequence cs)
{
    
}