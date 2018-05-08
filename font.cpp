#include "font.h"
#include <iostream>

int Font::draw(SDL_Renderer *renderer, const std::string &character, int x, int y)
{
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
    for (int i = 0; i < text.size(); ++i) {
        int x = x0 + character++ * cellWidth;

        if (i + 1 < text.size() && text[i] == '$' && text[++i] == '(') {
            int begin = i + 1;

            // parse extended token
            while (text[++i] != ')');

            if (this->draw(renderer, text.substr(begin, i - begin), x, y0) == -1)
                return -1;

            continue;
        }

        if (text[i] == ' ') {
            if (this->draw(renderer, "space", x, y0) == -1)
                return -1;
        } else {
            if (this->draw(renderer, std::string(1, text[i]), x, y0) == -1)
                return -1;
        }
    }

    return 0;
}