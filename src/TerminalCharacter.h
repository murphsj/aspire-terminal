#ifndef ASPIRE_TERMINAL_TERMINAL_CHARACTER
#define ASPIRE_TERMINAL_TERMINAL_CHARACTER

#include "TerminalColor.h"
#include <QColor>
#include <qobject.h>

/**
 * Represents a single Unicode character with ANSI escape code formatting options.
 */
struct TerminalCharacter
{
    enum Attribute {
        None = 0x0,
        Bold = 0x1,
        Faint = 0x2,
        Italic = 0x3,
        Underline = 0x4,
        Blink = 0x5,
        Strikethrough = 0x6
    };
    Q_DECLARE_FLAGS(Attributes, Attribute)
public:
    const static char LF = '\n';
    const static char CR = '\r';

    TerminalCharacter(QChar _v=QChar(' ')): character(_v) {};

    /** The 16-bit Unicode value of this character */
    QChar character;
    /** Bit flag of formatting modifiers on this character */
    Attributes attributes;
    /** The color to draw this character with */
    QColor fgColor {TerminalColor::DefaultForeground};
    /** The color to draw the background of this character with */
    QColor bgColor {TerminalColor::DefaultBackground};

    friend bool operator==(const TerminalCharacter& lhs, const TerminalCharacter& rhs)
    {
        return (
            lhs.character == rhs.character      &&
            lhs.attributes == rhs.attributes    &&
            lhs.fgColor == rhs.fgColor          &&
            lhs.bgColor == rhs.bgColor
        );
    };
};



#endif