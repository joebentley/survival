#ifndef SURVIVAL_FONTCOLOR_H
#define SURVIVAL_FONTCOLOR_H

#include <SDL.h>

#include <string>
#include <unordered_map>

/// rgba color structure
struct Color {
    Color() : r(0xFF), g(0xFF), b(0xFF), a(0xFF) {}
    Color(Uint8 r, Uint8 g, Uint8 b) : Color(r, g, b, 0xFF) {}
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : r(r), g(g), b(b), a(a) {}

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    friend Color operator*(Color lhs, float l) {
        lhs.r = (Uint8)((float)lhs.r * l);
        lhs.g = (Uint8)((float)lhs.g * l);
        lhs.b = (Uint8)((float)lhs.b * l);
        return lhs;
    }
};

namespace FontColor {
    /// Get the rgba color corresponding to the string, will raise exception if not found
    Color getColor(const std::string& colorStr);
    /// Return the rgba string to color map
    std::unordered_map<std::string, Color> &getColorMap();
}


#endif //SURVIVAL_FONTCOLOR_H
