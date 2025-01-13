#ifndef ASPIRE_TERMINAL_ESCAPE_SEQUENCE
#define ASPIRE_TERMINAL_ESCAPE_SEQUENCE

#include <qglobal.h>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class EscapeSequence
{
    /* 
     * This is based on Terminalpp's escape code parsing.
     * Note that control sequences and operating system control sequences are not actually part of the escape sequence;
     * the escape sequence ends with the control sequence introducer and it has different parsing rules
     */

public:
    enum SequenceType {
        ESC = 0x0,
        CSI = 0x1,
        OSC = 0x2
    };

    typedef std::variant<const char*, int> SequenceParameter;
    
    
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
        return static_cast<int>(c - '0');
    }

    /**
      * Reads an escape sequence at the given index of the given string to this object 
      * Precondition: seq.at(index-1) is an escape sequence introducer character
      */
    ssize_t read(std::string_view seq, ssize_t index);
    SequenceType getSequenceType() { return m_sequenceType; }
    std::vector<SequenceParameter> getParameters() { return m_parameters; }
    char getFinalChar() { return m_finalChar; }
    /** Returns if the sequence should be ignored, either because it is not supported or because it is malformed */
    bool isInvalid() { return m_invalid; }
    void debugInfo();

private:
    ssize_t readControlSequence(std::string_view seq, ssize_t index);

    SequenceType m_sequenceType {SequenceType::ESC};
    std::vector<SequenceParameter> m_parameters {};
    std::vector<char> m_intermediateChars {};
    char m_finalChar;
    bool m_invalid {false};
};

#endif