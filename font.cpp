#include "font.h"
#include <iostream>

enum FontColor fontColorFromString(const std::string &colorStr) {
    if (colorStr == "white") {
        return FONT_WHITE;
    } else if (colorStr == "yellow") {
        return FONT_YELLOW;
    }

    return FONT_INVALID;
}

void Font::setFontColor(enum FontColor color) {
    if (color == FONT_WHITE) {
        SDL_SetTextureColorMod(texture.texture, 0xFF, 0xFF, 0xFF);
    } else if (color == FONT_YELLOW) {
        SDL_SetTextureColorMod(texture.texture, 0xFF, 0xFF, 0);
    }
}

int Font::draw(SDL_Renderer *renderer, const std::string &character, int x, int y, enum FontColor color)
{
    setFontColor(color);

    for (int i = 0; i < characters.size(); ++i) {
        if (characters[i] == character) {
            int cellX = i % numPerRow;
            int cellY = i / numPerRow;

            SDL_Rect srcRect = { cellX * cellWidth, cellHeight * cellY, cellWidth, cellHeight };
            SDL_Rect destRect = { x, y, cellWidth, cellHeight };
            texture.render(renderer, &srcRect, &destRect);
            return 0;
        }
    }

    std::cerr << "Invalid rendering token: " << character << std::endl;
    return -1;
}

int Font::drawText(SDL_Renderer *renderer, const std::string &text, int x0, int y0)
{
    int character = 0;
    enum FontColor color = FONT_WHITE;

    for (int i = 0; i < text.size(); ++i) {
        int x = x0 + character * cellWidth;

        // parse extended token
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            int begin = i + 1;
            while (text[++i] != ')');
            
            character++;
            if (this->draw(renderer, text.substr(begin, i - begin), x, y0, color) == -1)
                return -1;

            continue;
        }

        // parse color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '[') {
            ++i;
            int begin = i + 1;
            while (text[++i] != ']');

            std::string fontStr = text.substr(begin, i - begin);
            color = fontColorFromString(fontStr);
            if (color == FONT_INVALID) {
                std::cerr << "Invalid font color: " << fontStr << std::endl;
                return -1;
            }
            
            continue;
        }

        character++;
        if (text[i] == ' ') {
            if (this->draw(renderer, "space", x, y0, color) == -1)
                return -1;
        } else {
            if (this->draw(renderer, std::string(1, text[i]), x, y0, color) == -1)
                return -1;
        }
    }

    return 0;
}