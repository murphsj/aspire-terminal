#ifndef ASPIRE_TERMINAL_TERMINAL_CHARACTER
#define ASPIRE_TERMINAL_TERMINAL_CHARACTER

#include <QColor>
#include <qobject.h>

/**
 * Represents a single Unicode character with ANSI escape code formatting options.
 * Just holds a QChar for now.
 */
struct TerminalCharacter
{
    enum CharacterAttribute {
        None = 0x0,
        Bold = 0x1,
        Faint = 0x2,
        Italic = 0x3,
        Underline = 0x4,
        Blink = 0x5
    };
    Q_DECLARE_FLAGS(CharacterAttributes, CharacterAttribute)
public:
    TerminalCharacter(QChar _v=QChar(' ')): character(_v) {};

    /** The 16-bit Unicode value of this character */
    QChar character;
    /** Bit flag of formatting modifiers on this character */
    CharacterAttributes attributes;
    /** The color to draw this character with */
    QColor fgColor;
    /** The color to draw the background of this character with */
    QColor bgColor;
};



#endif