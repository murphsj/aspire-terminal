#ifndef ASPIRE_TERMINAL_TEXT_BUFFER
#define ASPIRE_TERMINAL_TEXT_BUFFER

#include <QBitArray>
#include <QColor>

#include <cstddef> // for std::size_t

#include "TerminalCharacter.h"

/**
 * A buffer of one or more lines of text.
 */
class TextBuffer
{
    enum WriteMode {
        None = 0x0,
        WrapLines = 0x1,
        Insert = 0x2
    };
    Q_DECLARE_FLAGS(WriteModes, WriteMode)

public:
    TextBuffer();
    TextBuffer(std::size_t columns, std::size_t lines);
    ~ TextBuffer();
    /** Writes a TerminalCharacter to the buffer at the cursor position. */
    void write(TerminalCharacter c);
    /** Changes the size of the buffer, */
    void resize(std::size_t columns, std::size_t lines);
    /** Moves cursor down one line. */
    void nextLine();
    /** Adds a blank character at the cursor, offsetting other characters */
    void insert(int length=1);
    /** Moves the cursor left by n columns */
    void cursorLeft(std::size_t columnCount=std::size_t{1});
    /** Moves the cursor right by n columns */
    void cursorRight(std::size_t columnCount=std::size_t{1});
    /** Moves the cursor up by n lines */
    void cursorUp(std::size_t lineCount=std::size_t{1});
    /** Moves the cursor down by n lines */
    void cursorDown(std::size_t lineCount=std::size_t{1});
    /** Moves cursor to the beginning of the current line. */
    void toStartOfLine();
    /** Returns whether the given WriteMode flag is enabled. */
    bool hasWriteMode(WriteMode w);
    /** Sets whether the given WriteMode flag is enabled. */
    void setWriteMode(WriteMode w, bool on);
    /** Sets the color to assign to newly inserted characters. */
    void setFgColor(QColor color);
    /** Sets the background color to assign to newly inserted characters. */
    void setBgColor(QColor color);
    /** Sets whether a character attribute will be applied to newly inserted characters. */
    void setAttribute(TerminalCharacter::CharacterAttribute attr, bool on);
    /** Carries out an attribute specified by the 'Set Character Attribute' (SGR) control sequence. */
    void applyCharAttribute(int id);
    /** Returns the column size of the buffer. */
    std::size_t getColumns() { return m_columns; }
    /** Returns the line size of the buffer. */
    std::size_t getLines() { return m_lines; }

    QVector<TerminalCharacter>* begin() { return m_characterData; }
    QVector<TerminalCharacter>* end() { return m_characterData + m_lines; }
private:
    /** Sets character formatting to default. */
    void resetAttributes();
    /** Amount of characters for each line in the buffer. */
    std::size_t m_columns {1};
    /** Amount of lines in the buffer. */
    std::size_t m_lines {1};
    /** Column number of the cursor. */
    std::size_t m_cursorX {0};
    /** Line number of the cursor. */
    std::size_t m_cursorY {0};
    /** Bit field for options affecting how text is inserted into the buffer. */
    WriteModes m_writeMode {None};
    /** Array which stores the text in each line. */
    QVector<TerminalCharacter>* m_characterData;
    /** Foreground color to apply to text inserted into the buffer. */
    QColor m_fgColor {TerminalColor::DefaultForeground};
    /** Background color to apply to text inserted into the buffer. */
    QColor m_bgColor {TerminalColor::DefaultBackground};
    /** Attributes to apply to text inserted into the buffer. */
    TerminalCharacter::CharacterAttributes m_attributes;
};

#endif