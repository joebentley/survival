
#ifndef FONT_H_
#define FONT_H_

#include "texture.h"
#include <SDL2/SDL.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

std::unordered_map<std::string, Color> colorMap = {
    {"white", (struct Color) { 0xFF, 0xFF, 0xFF, 0xFF }},
    {"yellow", (struct Color) { 0xFF, 0xFF, 0, 0xFF }},
    {"grey", (struct Color) { 0x55, 0x55, 0x55, 0xFF }},
    {"transparent", (struct Color) { 0, 0, 0, 0 }}
};

class Font {
    Texture texture;
    int numPerRow;
    int cellWidth;
    int cellHeight;
public:
    std::vector<std::string> characters;

    Font(const Texture &texture, int cellWidth, int cellHeight, int numPerRow, const std::string &characters)
        : texture(texture), numPerRow(numPerRow), cellWidth(cellWidth), cellHeight(cellHeight)
    {
        std::istringstream iss(characters);
        std::copy(std::istream_iterator<std::string>(iss),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(this->characters));
    }

    void setFontColor(Color color);
    int draw(SDL_Renderer *renderer, const std::string &character, int x, int y, Color fColor)
    {
        return draw(renderer, character, x, y, fColor, (struct Color) { 0, 0, 0, 0 });
    }
    int draw(SDL_Renderer *renderer, const std::string &character, int x, int y, Color fColor, Color bColor);
    int drawText(SDL_Renderer *renderer, const std::string &text, int x, int y);
};

#endif // FONT_H_