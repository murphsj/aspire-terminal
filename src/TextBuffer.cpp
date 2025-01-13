#include "TextBuffer.h"

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
    m_characterData[m_cursorY][m_cursorX] = c;

    c.attributes = m_attributes;
    c.fgColor = m_fgColor;
    c.bgColor = m_bgColor;

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