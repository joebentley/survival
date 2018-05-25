#ifndef SURVIVAL_FONTCOLOR_H
#define SURVIVAL_FONTCOLOR_H

#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <string>
#include <unordered_map>

struct Color {
    Color() : r(0xFF), g(0xFF), b(0xFF), a(0xFF) {}
    Color(Uint8 r, Uint8 g, Uint8 b) : Color(r, g, b, 0xFF) {}
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : r(r), g(g), b(b), a(a) {}

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

class FontColor {
public:
    static Color getColor(const std::string& colorStr);

    static std::unordered_map<std::string, Color> &getColorMap();

private:
    static std::unordered_map<std::string, Color> mColorMap;
};


#endif //SURVIVAL_FONTCOLOR_H
