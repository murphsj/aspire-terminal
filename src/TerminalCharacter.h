#ifndef ASPIRE_TERMINAL_TERMINAL_CHARACTER
#define ASPIRE_TERMINAL_TERMINAL_CHARACTER

#include <qobject.h>

/**
 * Represents a single Unicode character with ANSI escape code formatting options.
 * Just holds a QChar for now.
 */
struct TerminalCharacter
{
public:
    TerminalCharacter(QChar _v=QChar(' ')): character(_v) {};

    /* The 16-bit Unicode value of this character */
    QChar character;
};



#endif