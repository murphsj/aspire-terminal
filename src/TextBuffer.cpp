#include "TextBuffer.h"
#include "TerminalColor.h"
#include <qdebug.h>

TextBuffer::TextBuffer(): TextBuffer(1, 1)
{
    
}

TextBuffer::~TextBuffer()
{
    delete[] m_characterData;
}

TextBuffer::TextBuffer(std::size_t columns, std::size_t lines)
    :m_columns(columns)
    ,m_lines(lines)
{
    m_characterData = new QVector<TerminalCharacter>[lines];
    for (int i {0}; i < m_lines; i++) {
        m_characterData[i] = QVector<TerminalCharacter>(m_columns);
    }
}

void TextBuffer::toStartOfLine()
{
    m_cursorX = 0;
}

void TextBuffer::cursorDown(std::size_t lineCount)
{
    if (m_cursorX >= m_columns)
        m_cursorX = 0;
    m_cursorY = qMin(m_cursorY + lineCount, m_lines-1);
}

void TextBuffer::cursorUp(std::size_t lineCount)
{
    if (m_cursorX >= m_columns)
        m_cursorX = 0;
    m_cursorY = qMax(m_cursorY - lineCount, std::size_t { 0 });
}

void TextBuffer::cursorRight(std::size_t charCount)
{
    // Cursor movement commands do not line wrap
    m_cursorX = qMax(m_cursorX + charCount, m_columns-1);
}

void TextBuffer::cursorLeft(std::size_t charCount)
{
    // Cursor movement commands do not line wrap
    m_cursorX = qMin(m_cursorX - charCount, std::size_t { 0 });
}

void TextBuffer::nextLine()
{
    if (m_cursorY >= m_lines) {
        // TODO: scroll down
    } else {
        m_cursorY += 1;
    }
}

void TextBuffer::insert(int length)
{
    static TerminalCharacter blank {};
    for (int i {0}; i < length; ++i) {
        m_characterData[m_cursorY].insert(m_cursorX, blank);
        ++m_cursorX;
    }
}

void TextBuffer::write(TerminalCharacter c)
{
    /*
    if (m_cursorX + 1 > m_columns) {
        toStartOfLine();
        // If WrapLines is off, keep writing at the start of the current line
        // TODO: clear current line? Happens in zsh atleast
        if (hasWriteMode(WriteMode::WrapLines)) nextLine();
    }*/

    if (hasWriteMode(WriteMode::Insert)) insert();

    if (m_characterData[m_cursorY].size() < m_cursorX + 1) {
        m_characterData[m_cursorY].resize(m_cursorX + 1);
    }

    c.attributes = m_attributes;
    c.fgColor = m_fgColor;
    c.bgColor = m_bgColor;

    m_characterData[m_cursorY][m_cursorX] = c;

    ++m_cursorX;
}

void TextBuffer::setWriteMode(WriteMode w, bool on)
{
    m_writeMode.setFlag(w, on);
}

bool TextBuffer::hasWriteMode(WriteMode w)
{
    return m_writeMode.testFlag(w);
}

void TextBuffer::setFgColor(QColor color)
{
    m_fgColor = color;
}

void TextBuffer::setBgColor(QColor color)
{
    m_bgColor = color;
}

void TextBuffer::setAttribute(TerminalCharacter::CharacterAttribute attr, bool on)
{
    m_attributes.setFlag(attr, on);
}

void TextBuffer::resetAttributes()
{
    setFgColor(TerminalColor::DefaultForeground);
    setBgColor(TerminalColor::DefaultBackground);
}

void TextBuffer::applyCharAttribute(int id)
{
    qDebug() << "Applying char attribute " << id;
    switch (id) {
    case 0:     resetAttributes();  break;
    /* 16-bit Colors FG */
    case 30:    setFgColor(TerminalColor::Black);  break;
    case 31:    setFgColor(TerminalColor::Red);    break;
    case 32:    setFgColor(TerminalColor::Green);  break;
    case 33:    setFgColor(TerminalColor::Yellow); break;
    case 34:    setFgColor(TerminalColor::Blue);   break;
    case 35:    setFgColor(TerminalColor::Magenta);break;
    case 36:    setFgColor(TerminalColor::Cyan);   break;
    case 37:    setFgColor(TerminalColor::White);  break;
    case 39:    setFgColor(TerminalColor::DefaultForeground); break;
    /* 16-bit Colors BG */
    case 40:    setBgColor(TerminalColor::Black);  break;
    case 41:    setBgColor(TerminalColor::Red);    break;
    case 42:    setBgColor(TerminalColor::Green);  break;
    case 43:    setBgColor(TerminalColor::Yellow); break;
    case 44:    setBgColor(TerminalColor::Blue);   break;
    case 45:    setBgColor(TerminalColor::Magenta);break;
    case 46:    setBgColor(TerminalColor::Cyan);   break;
    case 47:    setBgColor(TerminalColor::White);  break;
    case 49:    setBgColor(TerminalColor::DefaultBackground); break;
    /* Bright Colors FG */
    case 90:    setFgColor(TerminalColor::BrightBlack);  break;
    case 91:    setFgColor(TerminalColor::BrightRed);    break;
    case 92:    setFgColor(TerminalColor::BrightGreen);  break;
    case 93:    setFgColor(TerminalColor::BrightYellow); break;
    case 94:    setFgColor(TerminalColor::BrightBlue);   break;
    case 95:    setFgColor(TerminalColor::BrightMagenta);break;
    case 96:    setFgColor(TerminalColor::BrightCyan);   break;
    case 97:    setFgColor(TerminalColor::BrightWhite);  break;
    /* Bright Colors BG */
    case 100:   setBgColor(TerminalColor::BrightBlack);  break;
    case 101:   setBgColor(TerminalColor::BrightRed);    break;
    case 102:   setBgColor(TerminalColor::BrightGreen);  break;
    case 103:   setBgColor(TerminalColor::BrightYellow); break;
    case 104:   setBgColor(TerminalColor::BrightBlue);   break;
    case 105:   setBgColor(TerminalColor::BrightMagenta);break;
    case 106:   setBgColor(TerminalColor::BrightCyan);   break;
    case 107:   setBgColor(TerminalColor::BrightWhite);  break;
    }
}