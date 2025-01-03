#include <qdebug.h>

#include "EscapeSequence.h"

ssize_t EscapeSequence::parse(std::string_view seq, ssize_t index)
{
    int len {0};
    while (true) {
        ++index;
        ++len;
        char c {seq.at(index)};
        if (!c) {
            qDebug() << "Parsed malformed ESC sequence: end of input without terminating character";
            m_malformed = true;
            return len;
        } else if (EscapeSequence::isEscFinal(c)) {
            break;
        } else if (EscapeSequence::isEscIntermediate(c)) {

        } else {
            qDebug() << "Parsed malformed ESC sequence: invalid intermediate character " << c;
            m_malformed = true;
            return len;
        }
    } 
}