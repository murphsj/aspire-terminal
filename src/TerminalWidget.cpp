#include "TerminalWidget.h"
#include "EscapeSequence.h"
#include "TerminalCharacter.h"
#include "TerminalColor.h"

#include <QPainter>
#include <QKeyEvent>
#include <QFile>
#include <QRegularExpression>
#include <QStringListModel>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <cstddef>
#include <qnamespace.h>
#include <qwidget.h>

TerminalWidget::TerminalWidget(QWidget* parent, std::size_t width, std::size_t height)
    : QWidget(parent)
    , m_font("Monospace")
    , m_fontMetrics(m_font)
    , m_buffer(width, height)
    , m_pen()
    , m_blinkOn(false)
    , m_blinkTimer(this)
    , m_width(width)
    , m_height(height)
{
    // Style hint should result in a monospace font being found even if Monospace isn't available
    // (as is the case on Windows)
    m_font.setStyleHint(QFont::Monospace);
    setFocusPolicy(Qt::StrongFocus);

    m_completionModel = new ShellCompletionModel(this);

    m_completer = new ShellCompleter(m_completionModel, this);
    m_completer->setWidget(this);
    m_completer->setCompletionColumn(0);
    m_completer->setCompletionRole(Qt::DisplayRole);
    m_completer->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    m_completer->popup()->setFont(m_font);

    connect(&m_blinkTimer, &QTimer::timeout, this, &TerminalWidget::blinkEvent);
    m_blinkTimer.start(TerminalWidget::BlinkInterval);

    connect(&m_pty, &Pty::recieved, this, &TerminalWidget::recievedFdData);
    m_pty.start(width, height);

    connect(m_completer, QOverload<const QModelIndex&>::of(&QCompleter::activated), this, &TerminalWidget::completionActivated);
}

QRect TerminalWidget::getCharRect(std::size_t charX, std::size_t charY)
{
    int charWidth { m_fontMetrics.averageCharWidth() };
    int charHeight { m_fontMetrics.height() };

    return QRect(charX * charWidth, charY * charHeight, charWidth, charHeight);
}

void TerminalWidget::paintEvent(QPaintEvent* event)
{
    QRect region { contentsRect() };
    QPainter painter(this);
    painter.setFont(m_font);
    paintBackground(painter, region);
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

void TerminalWidget::blinkEvent()
{
    m_blinkOn = !m_blinkOn;
    update();
}

void TerminalWidget::updateCompletion()
{
    QString prompt {m_buffer.getCompletion(m_buffer.getPrompt())};
    if (prompt.isEmpty()) {
        m_completer->popup()->hide();
        return;
    }
    m_completer->setCompletionPrefix(prompt);
    m_completer->setCurrentRow(0);
    QRect rect { getCharRect(m_buffer.getCursorX(), m_buffer.getCursorY()) };
    rect.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(rect);
}

void TerminalWidget::completionActivated(const QModelIndex& completion)
{
    static QRegularExpression whitespace {"\\s+"};

    QString name { completion.data().toString() };
    QString prompt { m_buffer.getPrompt() };
    if (!prompt.endsWith(' ')) {
        QStringList promptWords { prompt.split(whitespace, Qt::SkipEmptyParts) };
        QString lastWord { promptWords.at(promptWords.length()-1) };
        for (int i = 0; i < lastWord.length(); i++) m_pty.send("\b", 1);
    }
    
    m_pty.send(name.toUtf8().constData(), name.length());
}

void TerminalWidget::paintBackground(QPainter& painter, QRect& region)
{
    painter.fillRect(region, TerminalColor::DefaultBackground);
}

void TerminalWidget::paintAllCharacters(QPainter& painter, QRect& region)
{
    QPoint topLeft = region.topLeft();
    
    std::size_t rows { 0 };
    std::size_t cols { 0 };

    //std::size_t maxCols { qMin(static_cast<std::size_t>(region.width() / charWidth), m_buffer.getColumns()) };

    for (QVector<TerminalCharacter> line : m_buffer) {
        for (TerminalCharacter c : line) {
            QRect drawRect = getCharRect(cols, rows);
            bool drawCursorHighlight = { cols == m_buffer.getCursorX() && rows == m_buffer.getCursorY() && m_blinkOn };
            paintCharacter(painter, drawRect, c, drawCursorHighlight);
            cols++;
        }
        cols = 0;
        rows++;
    }
}

void TerminalWidget::paintCharacter(QPainter& painter, QRect& region, TerminalCharacter c, bool drawCursorHighlight)
{
    QColor fg {};
    QColor bg {};
    if (drawCursorHighlight) {
        fg = c.bgColor;
        bg = c.fgColor;
    } else {
        fg = c.fgColor;
        bg = c.bgColor;
    }
    m_font.setBold(c.attributes.testFlag(TerminalCharacter::Bold));
    m_font.setItalic(c.attributes.testFlag(TerminalCharacter::Italic));
    m_font.setUnderline(c.attributes.testFlag(TerminalCharacter::Underline));
    m_font.setStrikeOut(c.attributes.testFlag(TerminalCharacter::Strikethrough));
    painter.setFont(m_font);
    painter.setPen(fg);

    painter.fillRect(region, bg);
    painter.drawText(region, c.character);
}

void TerminalWidget::recievedFdData(std::string_view output)
{
    ssize_t i {0};
    while (i < output.length()) {
        ssize_t newIndex {parse(output, i)};
        i = newIndex;
    }

    updateCompletion();
    update();
}

QSize TerminalWidget::sizeHint() const
{
    int charWidth { m_fontMetrics.averageCharWidth() };
    int charHeight { m_fontMetrics.height() };
    return QSize(m_width * charWidth, m_height * charHeight);
}

ssize_t TerminalWidget::parse(std::string_view output, ssize_t index)
{
    const char& ch = output.at(index);
    if (ch == EscapeSequence::ESC_INTRODUCER) {
        return parseEscapeSequence(output, ++index);
    } else if (ch == TerminalCharacter::LF) {
        m_buffer.lineFeed();
        return ++index;
    } else if (ch == TerminalCharacter::CR) {
        m_buffer.carriageReturn();
        return ++index;
    } else if (ch == TerminalCharacter::BS) {
        m_buffer.backspace();
        return ++index;
    } else if (ch == TerminalCharacter::DEL) {
        m_buffer.write(TerminalCharacter {});
        return ++index;
    } else if (ch == TerminalCharacter::BELL) {
        // Unimplemented for now
        return ++index;
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
            m_buffer.applyControlSequence(escSequence);
            break;
        case EscapeSequence::SequenceType::OSC:
            break;
        }
    }

    return newIndex;
}

void TerminalWidget::applyEscapeSequence(EscapeSequence cs)
{
    
}