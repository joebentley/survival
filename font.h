
#ifndef FONT_H_
#define FONT_H_

#include "texture.h"
#include <SDL2/SDL.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

enum FontColor {
    FONT_WHITE,
    FONT_YELLOW,
    FONT_INVALID
};

enum FontColor fontColorFromString(const std::string &colorStr);

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

    void setFontColor(enum FontColor color);
    int draw(SDL_Renderer *renderer, const std::string &character, int x, int y, enum FontColor color);
    int drawText(SDL_Renderer *renderer, const std::string &text, int x, int y);
};

#endif // FONT_H_