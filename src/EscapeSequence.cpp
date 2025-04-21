#include <qdebug.h>
#include <variant>

#include "TerminalCharacter.h"
#include "EscapeSequence.h"

ssize_t EscapeSequence::read(std::string_view seq, ssize_t index)
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
            m_invalid = true;
            return ++index;
        }
    }

    if (c == CS_INTRODUCER) {
        if (!m_intermediateChars.empty()) {
            qDebug() << "Parsed malformed ESC sequence: CSI with intermediate arguments";
            m_invalid = true;
            return ++index;
        }
        return readControlSequence(seq, index);
    } else if (c == OSC_INTRODUCER) {
        if (!m_intermediateChars.empty()) {
            qDebug() << "Parsed malformed ESC sequence: OSC with intermediate arguments";
            m_invalid = true;
            return ++index;
        }
        return readOperatingSystemControl(seq, index);
    }

    m_finalChar = c;
    return index;
}

ssize_t EscapeSequence::readControlSequence(std::string_view seq, ssize_t index)
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
            while (index < seq.size()) {
                c = seq.at(index);
                if (!isDigit(c)) break;
                numericParameter *= 10;
                numericParameter += digitCharToInt(c);
                ++index;
            }
            m_parameters.push_back(numericParameter);
            // If there's a semicolon at the end, skip it
            if (c == ';') {
                ++index;
            }
        } else {
            // This is a string parameter; we don't support any control sequences that take strings so it's invalid
            m_invalid = true;
            ++index;
        }
    }
    // Parameter list is followed by optional sequence of intermediate characters
    while (index < seq.size()) {
        char c = seq.at(index);
        if (!isCsIntermediate(c)) break;
        m_intermediateChars.push_back(c);
        ++index;
    }

    if (index >= seq.size() || !isCsFinal(seq.at(index))) {
        m_invalid = true;
        qDebug() << "Parsed malformed control sequence: ended without terminating character";
    } else {
        m_finalChar = seq.at(index);
    }

    m_sequenceType = SequenceType::CSI;

    return ++index;
}

ssize_t EscapeSequence::readOperatingSystemControl(std::string_view seq, ssize_t index)
{
    // We ignore all OSCs. OSCs are terminated by the Bell character, so just skip to the next occurence of it
    m_invalid = true;
    while (index < seq.size()) {
        char c = seq.at(index);
        if (c == TerminalCharacter::BELL) break;
        ++index;
    }

    return ++index;
}

void EscapeSequence::debugInfo()
{
    qDebug() << "Parameters:";
    for (SequenceParameter param : m_parameters) {
        std::visit([](auto&& p) {qDebug() << p;}, param);
    }
    qDebug() << "Final char:" << m_finalChar;
}

