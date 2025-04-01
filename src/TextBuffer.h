#ifndef ASPIRE_TERMINAL_TEXT_BUFFER
#define ASPIRE_TERMINAL_TEXT_BUFFER

#include <QBitArray>
#include <QColor>

#include <cstddef> // for std::size_t

#include "EscapeSequence.h"
#include "TerminalCharacter.h"

/**
 * A buffer of one or more lines of text.
 */
class TextBuffer
{
    enum WriteMode {
        None = 0x0,
        Insert = 0x1
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
    /** Adds a blank character at the cursor, offsetting other characters. */
    void insert(int length=1);
    /** Sets which column the cursor is on. */
    void setCursorX(std::size_t pos);
    /** Sets which line the cursor is on. */
    void setCursorY(std::size_t pos);
    /** Returns which column the cursor is on. */
    std::size_t getCursorX();
    /** Returns which line the cursor is on. */
    std::size_t getCursorY();
    /** Attempts to find the user-entered string at the cursor in the Bash shell. */
    QString getPrompt();
    /** Returns the string to be used for shell completion */
    QString getCompletion();
    /** Sets the column and line of the cursor. */
    void setCursorPosition(std::size_t x, std::size_t y);
    /** Parses an LF character; moves the cursor down one line, scrolling if neccecary. */
    void lineFeed();
    /** Parses a CR character; moves the cursor to the start of the current line. */
    void carriageReturn();
    /** Moves the cursor left by n columns */
    void cursorLeft(std::size_t columnCount=std::size_t{1});
    /** Moves the cursor right by n columns */
    void cursorRight(std::size_t columnCount=std::size_t{1});
    /** Moves the cursor up by n lines */
    void cursorUp(std::size_t lineCount=std::size_t{1});
    /** Moves the cursor down by n lines */
    void cursorDown(std::size_t lineCount=std::size_t{1});
    /** Moves the cursor to the left 1 character, line wrapping. */
    void backspace();
    /** Moves cursor to the start of the next nth line. */
    void cursorNextLine(std::size_t lineCount=std::size_t{1});
    /** Moves cursor to the start of the previous nth line. */
    void cursorPreviousLine(std::size_t lineCount=std::size_t{1});
    /** Clears every character from the cursor position to the end of the buffer. */
    void eraseToEnd();
    /** Clears every character from the start of the buffer to the cursor position. */
    void eraseFromStart();
    /** Clears every character in the buffer. */
    void eraseAll();

    void eraseLineToEnd();
    void eraseLineFromStart();
    void eraseLineAll();

    /** Returns whether the given WriteMode flag is enabled. */
    bool hasWriteMode(WriteMode w);
    /** Sets whether the given WriteMode flag is enabled. */
    void setWriteMode(WriteMode w, bool on);
    /** Sets the color to assign to newly inserted characters. */
    void setFgColor(QColor color);
    /** Sets the background color to assign to newly inserted characters. */
    void setBgColor(QColor color);
    /** Sets whether a character attribute will be applied to newly inserted characters. */
    void setAttribute(TerminalCharacter::Attribute attr, bool on);
    /** Carries out an attribute specified by the 'Set Character Attribute' (SGR) control sequence. */
    void applyCharAttribute(int id);
    /** Carries out a control sequence command. */
    void applyControlSequence(EscapeSequence cs);
    /** Returns the column size of the buffer. */
    std::size_t getColumns() { return m_columns; }
    /** Returns the line size of the buffer. */
    std::size_t getLines() { return m_lines; }


    QVector<TerminalCharacter>* begin() { return m_characterData; }
    QVector<TerminalCharacter>* end() { return m_characterData + m_lines; }
private:
    /** Sets all characters from the start to the end position to the given character. If the given character is blank, erases all characters in range. */
    void fillInRange(TerminalCharacter c, std::size_t startX, std::size_t startY, std::size_t endX, std::size_t endY);
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
    WriteModes m_writeMode {};
    /** Array which stores the text in each line. */
    QVector<TerminalCharacter>* m_characterData;
    /** Foreground color to apply to text inserted into the buffer. */
    QColor m_fgColor {TerminalColor::DefaultForeground};
    /** Background color to apply to text inserted into the buffer. */
    QColor m_bgColor {TerminalColor::DefaultBackground};
    /** Attributes to apply to text inserted into the buffer. */
    TerminalCharacter::Attributes m_attributes;
};

#endif