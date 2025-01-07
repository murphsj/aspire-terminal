#include <qdebug.h>

#include "EscapeSequence.h"

ssize_t EscapeSequence::parse(std::string_view seq, ssize_t index)
{
    char c;
    while (index < seq.size()) {
        c = seq.at(index);
        ++index;
        if (isEscFinal(c)) {
            break;
        } else if (isEscIntermediate(c)) {
            /*
             * Each character of an esc sequence usually represents one 'argument' or value
             * so we can just put each one as a SequenceParameter
             */
            m_intermediateChars.push_back(c);
        } else {
            qDebug() << "Parsed malformed ESC sequence: invalid intermediate character " << c;
            m_malformed = true;
            return ++index;
        }
    }

    if (c == CS_INTRODUCER) {
        if (!m_intermediateChars.empty()) {
            qDebug() << "Parsed malformed ESC sequence: CSI with intermediate arguments";
            m_malformed = true;
            return ++index;
        }
        return parseControlSequence(seq, index);
    }

    m_finalChar = c;
    return index;
}

ssize_t EscapeSequence::parseControlSequence(std::string_view seq, ssize_t index)
{
    // Control sequence starts with an optional list of parameters separated by ;
    while (index < seq.size()) {
        char c = seq.at(index);
        if (!isCsParameter(c)) break;
        if (c == ';') {
            // This is a blank parameter, assign default value of 0
            ++index;
            m_parameters.push_back(0);
        } else if (isDigit(c)) {
            // This is a numeric parameter, parse a number out of it
            int numericParameter = 0;
            while (isDigit(c) && index < seq.size()) {
                numericParameter *= 10;
                numericParameter += digitCharToInt(c);
                ++index;
                c = seq.at(index);
            } 
            // If there's a semicolon at the end, skip it
            if (c == ';') {
                ++index;
            }
        } else {
            // This is a string parameter; we don't support any control sequences that take strings so it's invalid
            m_malformed = true;
            ++index;
        }
    }
    // Parameter list is followed by optional sequence of intermediate characters
    while (index < seq.size()) {
        char c = seq.at(index);
        if (isCsIntermediate(c)) break;
        m_intermediateChars.push_back(c);
        index++;
    }

    if (index >= seq.size() && !isCsFinal(seq.at(index))) {
        m_malformed = true;
        qDebug() << "Parsed malformed control sequence: ended without terminating character";
    } else {
        m_finalChar = seq.at(index);
    }

    return index;
}

