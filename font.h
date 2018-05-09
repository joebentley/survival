
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

Color getColor(const std::string& colorStr);

class Font {
    Texture texture;
    int numPerRow;
    int cellWidth;
    int cellHeight;
    SDL_Renderer *renderer;
public:
    std::vector<std::string> characters;

    Font(const Texture &texture, int cellWidth, int cellHeight, int numPerRow,
         const std::string &characters, SDL_Renderer *renderer)
        : texture(texture), numPerRow(numPerRow),
          cellWidth(cellWidth), cellHeight(cellHeight), renderer(renderer)
    {
        std::istringstream iss(characters);
        std::copy(std::istream_iterator<std::string>(iss),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(this->characters));
    }

    void setFontColor(Color color);
    int draw(const std::string &character, int x, int y, Color fColor)
    {
        return draw(character, x, y, fColor, (struct Color) { 0, 0, 0, 0 });
    }
    int draw(const std::string &character, int x, int y, Color fColor, Color bColor);
    int drawText(const std::string &text, int x, int y);
};

int getFontStringLength(const std::string& string);

#endif // FONT_H_