#ifndef ASPIRE_TERMINAL_COLOR
#define ASPIRE_TERMINAL_COLOR

#include <QColor>

namespace TerminalColor
{
    /* 16-bit colors */
    constexpr QColor Black {0, 0, 0};
    constexpr QColor Red {170, 0, 0};
    constexpr QColor Green {0, 170, 0};
    constexpr QColor Yellow {170, 85, 0};
    constexpr QColor Blue {0, 0, 170};
    constexpr QColor Magenta {170, 0, 170};
    constexpr QColor Cyan {0, 170, 170};
    constexpr QColor White {229, 229, 229};
    constexpr QColor BrightBlack {85, 85, 85};
    constexpr QColor BrightRed {255, 85, 85};
    constexpr QColor BrightGreen {85, 255, 85};
    constexpr QColor BrightYellow {255, 255, 85};
    constexpr QColor BrightBlue {85, 85, 255};
    constexpr QColor BrightMagenta {255, 85, 255};
    constexpr QColor BrightCyan {85, 255, 255};
    constexpr QColor BrightWhite {255, 255, 255};

    constexpr QColor DefaultForeground {White};
    constexpr QColor DefaultBackground {Black};

    QColor readColor16(int id);
    QColor readColor256(int id);
    QColor readColorRGB(int r, int g, int b);
};

#endif 