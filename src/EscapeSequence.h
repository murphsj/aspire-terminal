#ifndef ASPIRE_TERMINAL_ESCAPE_SEQUENCE
#define ASPIRE_TERMINAL_ESCAPE_SEQUENCE

#include <qglobal.h>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class EscapeSequence
{
    typedef std::variant<std::string, int> SequenceParameter;

public:
    /* 
     * This is based on Terminalpp's escape code parsing.
     * Note that control sequences are not actually part of the escape sequence;
     * the escape sequence ends with the control sequence introducer and it has different parsing rules
     */
    
    static const char ESC_INTRODUCER = '\033';
    static const char CS_INTRODUCER = '[';

    /* Returns whether or not the character is a valid intermediate character in an escape sequence. */
    static bool isEscIntermediate(char c)
    {
        // Matches !"#$%&'()*+,-./ and space
        return (c >= 0x20) && (c <= 0x2f);
    }

    /* Returns whether or not the character is a valid final character in an escape sequence. */
    static bool isEscFinal(char c)
    {
        // Matches A-Z, a-z, 0-9, and :;<=>?
        return (c >= 0x30) && (c <= 0x7e);
    }

    /* Returns whether or not the character is a valid parameter character in a control sequence. */
    static bool isCsParameter(char c)
    {
        // Matches 0-9 and :;<=>?
        return (c >= 0x30) && (c <= 0x3f);
    }

    /* Returns whether or not the character is a valid intermediate character in a control sequence. */
    static bool isCsIntermediate(char c)
    {
        // Matches !"#$%&'()*+,-./ and space
        return (c >= 0x20) && (c <= 0x2f);
    }

    /* Returns whether or not the character is a valid final character in a control sequence. */
    static bool isCsFinal(char c)
    {
        // Matches A-Z, a-z, and most symbols
        return (c >= 0x40);
    }

    static bool isDigit(char c)
    {
        return (c >= 0x30) && (c <= 0x39);
    }

    static int digitCharToInt(char c)
    {
        assert(isDigit(c));
        return static_cast<int>('0' - c);
    }

    ssize_t parse(std::string_view seq, ssize_t index);
    char getFirstChar();
    char getFinalChar();
    bool isMalformed();

private:
    ssize_t parseControlSequence(std::string_view seq, ssize_t index);

    char m_firstChar;
    std::vector<SequenceParameter> m_parameters {};
    std::vector<char> m_intermediateChars {};
    char m_finalChar;
    bool m_malformed {false};
};

#endif