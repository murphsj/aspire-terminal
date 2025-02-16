#include "TextBuffer.h"
#include "EscapeSequence.h"
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
    ,m_writeMode(WrapLines)
{
    m_characterData = new QVector<TerminalCharacter>[lines];
    for (int i {0}; i < m_lines; i++) {
        m_characterData[i] = QVector<TerminalCharacter>(columns);
    }
}

void TextBuffer::setCursorX(std::size_t pos)
{
    pos -= 1;
    m_cursorX = qMax(std::size_t {0}, qMin(pos, m_columns));
}

void TextBuffer::setCursorY(std::size_t pos)
{
    pos -= 1;
    m_cursorY = qMax(std::size_t {0}, qMin(pos, m_lines));
}

std::size_t TextBuffer::getCursorX()
{
    return m_cursorX;
}

std::size_t TextBuffer::getCursorY()
{
    return m_cursorY;
}

void TextBuffer::setCursorPosition(std::size_t x, std::size_t y)
{
    setCursorX(x); setCursorY(y);
}

void TextBuffer::lineFeed()
{
    setCursorY(m_cursorY + 2);
    qDebug() << "LF sent";
}

void TextBuffer::carriageReturn()
{
    setCursorX(1);
    qDebug() << "CR sent";
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
    m_cursorX = qMax(m_cursorX + charCount, m_columns-1);
}

void TextBuffer::cursorLeft(std::size_t charCount)
{
    m_cursorX = qMin(m_cursorX - charCount, std::size_t { 0 });
}

void TextBuffer::backspace()
{
    if (m_cursorX == 0) {
        if (m_cursorY > 0) {
            m_cursorY --;
        }
        m_cursorX = m_columns-1;
    } else {
        m_cursorX--;
    }
}

void TextBuffer::cursorNextLine(std::size_t lineCount)
{
    m_cursorX = 0;
    while (lineCount > 0 && m_cursorY < m_lines - 1) {
        ++m_cursorY;
        --lineCount;
    }
}

void TextBuffer::cursorPreviousLine(std::size_t lineCount)
{
    m_cursorX = 0;
    while (lineCount > 0 && m_cursorY > 0) {
        --m_cursorY;
        --lineCount;
    }
}

void TextBuffer::fillInRange(TerminalCharacter c, std::size_t startX, std::size_t startY, std::size_t endX, std::size_t endY)
{
    std::size_t startLoc {(startY * m_columns) + startX};
    std::size_t endLoc {(endY * m_columns) + endX};
    assert(endLoc >= startLoc);

    for (int y = startY; y <= endY; ++y) {
        QVector<TerminalCharacter>& line = m_characterData[y];
        std::size_t start {(y == startY) ? startX : 0};
        std::size_t end {(y == endY) ? endX : m_columns};

        if (line.size() < end) {
            line.resize(end);
        }

        for (int x = start; x < end; ++x) {
            line[x] = c;
        }
    }
}

void TextBuffer::eraseToEnd()
{
    fillInRange(TerminalCharacter {}, m_cursorX, m_cursorY, m_columns-1, m_lines-1);
}

void TextBuffer::eraseFromStart()
{
    fillInRange(TerminalCharacter {}, m_columns-1, m_lines-1, m_cursorX, m_cursorY);
}

void TextBuffer::eraseAll()
{
    fillInRange(TerminalCharacter {}, 0, 0, m_columns-1, m_lines-1);
}

void TextBuffer::eraseLineToEnd()
{
    fillInRange(TerminalCharacter {}, m_cursorX, m_cursorY, m_columns-1, m_cursorY);
}

void TextBuffer::eraseLineFromStart()
{
    fillInRange(TerminalCharacter {}, 0, m_cursorY, m_cursorX, m_cursorY);
}

void TextBuffer::eraseLineAll()
{
    fillInRange(TerminalCharacter {}, 0, m_cursorY, m_columns-1, m_cursorY);
}



void TextBuffer::insert(int length)
{
    static TerminalCharacter blank {};
    for (int i {0}; i < length; ++i) {
        ++m_cursorX;
        m_characterData[m_cursorY].insert(m_cursorX, blank);
    }
}

void TextBuffer::write(TerminalCharacter c)
{
    if (hasWriteMode(WriteMode::WrapLines) && (m_cursorX + 1 > m_columns))
    {
        carriageReturn();
        lineFeed();
    }

    if (hasWriteMode(WriteMode::Insert)) insert();

    if (m_characterData[m_cursorY].size() < m_cursorX + 1) {
        m_characterData[m_cursorY].resize(m_cursorX + 2);
    }

    c.attributes = m_attributes;
    c.fgColor = m_fgColor;
    c.bgColor = m_bgColor;

    m_characterData[m_cursorY][m_cursorX] = c;

    if (m_cursorX < m_columns) ++m_cursorX;
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

void TextBuffer::setAttribute(TerminalCharacter::Attribute attr, bool on)
{
    m_attributes.setFlag(attr, on);
}

void TextBuffer::resetAttributes()
{
    setFgColor(TerminalColor::DefaultForeground);
    setBgColor(TerminalColor::DefaultBackground);
    // It might be better to clear this in-place instead of making a new blank QFlag
    m_attributes = {};
}

void TextBuffer::applyCharAttribute(int id)
{
    switch (id) {
    case 0:     resetAttributes();  break;
    /* Formatting */
    case 1:     setAttribute(TerminalCharacter::Attribute::Bold,       true); break;
    case 2:     setAttribute(TerminalCharacter::Attribute::Faint,      true); break;
    case 3:     setAttribute(TerminalCharacter::Attribute::Italic,     true); break;
    case 4:     setAttribute(TerminalCharacter::Attribute::Underline,  true); break;
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

    default: qDebug() << "Non-supported SGR argument supplied: " << id; break;
    }
}

void TextBuffer::applyControlSequence(EscapeSequence cs)
{
    std::vector<EscapeSequence::SequenceParameter> params = cs.getParameters();

    switch (cs.getFinalChar()) {
    case '@':
        // ICH Insert Character
        insert(cs.getParameter(0, 1));
        break;
    case 'A':
        // CUU Cursor Up
        cursorUp(cs.getParameter(0, 1));
        break;
    case 'B':
        // CUD Cursor Down
        cursorDown(cs.getParameter(0, 1));
        break;
    case 'C':
        // CUF Cursor Forwards
        cursorRight(cs.getParameter(0, 1));
        break;
    case 'D':
        // CUB Cursor Backwards
        cursorLeft(cs.getParameter(0, 1));
        break;
    case 'E':
        // CNL Cursor Next Line
        cursorNextLine(cs.getParameter(0, 1));
        break;
    case 'F':
        // CPL Cursor Preceding Line
        cursorPreviousLine(cs.getParameter(0, 1));
        break;
    case 'G':
        // CHA Set Character Absolute
        setCursorX(cs.getParameter(0, 1));
        break;
    case 'H':
    case 'f':
        // CUP, HVP Set Cursor Position
        qDebug() << "Setting position:" << cs.getParameter(0, 1) << cs.getParameter(1, 1);
        setCursorPosition(cs.getParameter(1, 1), cs.getParameter(0, 1));
        break;
    case 'd':
        // VPA Line Position Absolute
        setCursorY(cs.getParameter(1, 1));
        break;
    case 'e':
        // VPR Line Position Relative
        setCursorY(m_cursorY + cs.getParameter(0, 1));
        break;
    case 'h':
        // DECSET Set Private Mode
        switch (cs.getParameter(1, -1)) {
        case 7:
            setWriteMode(WriteMode::WrapLines, true);   break;
        }
        break;
    case 'l':
        // DECRST Reset Private Mode
        switch (cs.getParameter(1, -1)) {
        case 7:
            setWriteMode(WriteMode::WrapLines, false);   break;
        }
        break;
    case '`':
        // HPA Character Position Absolute
        setCursorX(cs.getParameter(0, 1));
        break;
    case 'a':
        // HPR Character Position Relative
        setCursorX(m_cursorX + cs.getParameter(0, 1));
        break;
    case 'J':
        // ED Erase in Display
        switch (cs.getParameter(0, 0)) {
            case 0: eraseToEnd();       break;
            case 1: eraseFromStart();   break;
            case 2: eraseAll();         break;
            default: break;
        }
        break;
    case 'K':
        // EL Erase in Line
        switch (cs.getParameter(0, 0)) {
            case 0: eraseLineToEnd();       break;
            case 1: eraseLineFromStart();   break;
            case 2: eraseLineAll();         break;
            default: break;
        }
        break;
    case 'm':
        // SGR Set Graphics Rendition
        for (EscapeSequence::SequenceParameter param : params) {
            if (const int* value = std::get_if<int>(&param)) {
                applyCharAttribute(*value);
            }
        }
        break;
    }
}