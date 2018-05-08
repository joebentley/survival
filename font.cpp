#include "font.h"
#include <iostream>

void Font::setFontColor(Color c) {
    SDL_SetTextureColorMod(texture.texture, c.r, c.g, c.b);
}

int Font::draw(SDL_Renderer *renderer, const std::string &character, int x, int y, Color fColor, Color bColor)
{
    setFontColor(fColor);

    for (int i = 0; i < characters.size(); ++i) {
        if (characters[i] == character) {
            int cellX = i % numPerRow;
            int cellY = i / numPerRow;

            SDL_Rect srcRect = { cellX * cellWidth, cellY * cellHeight, cellWidth, cellHeight };
            SDL_Rect destRect = { x * cellWidth, y * cellHeight, cellWidth, cellHeight };

            SDL_SetRenderDrawColor(renderer, bColor.r, bColor.g, bColor.b, bColor.a);
            SDL_RenderFillRect(renderer, &destRect);

            texture.render(renderer, &srcRect, &destRect);
            return 0;
        }
    }

    std::cerr << "Invalid rendering token: " << character << std::endl;
    return -1;
}

int Font::drawText(SDL_Renderer *renderer, const std::string &text, int x0, int y)
{
    Color fColor = (struct Color) { 0xFF, 0xFF, 0xFF, 0xFF };
    Color bColor = (struct Color) { 0, 0, 0, 0 };
    int x = x0;

    for (int i = 0; i < text.size(); ++i) {
        // parse extended token
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            int begin = i + 1;
            while (text[++i] != ')');
            
            x++;
            if (this->draw(renderer, text.substr(begin, i - begin), x, y, fColor, bColor) == -1)
                return -1;

            continue;
        }

        // parse foreground color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '[') {
            ++i;
            int begin = i + 1;
            while (text[++i] != ']');

            std::string fontStr = text.substr(begin, i - begin);
            if (colorMap.find(fontStr) == colorMap.end()) {
                std::cerr << "Invalid foreground font color: " << fontStr << std::endl;
                return -1;
            }
            fColor = colorMap[fontStr];

            continue;
        }

        // parse background color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '{') {
            ++i;
            int begin = i + 1;
            while (text[++i] != '}');

            std::string fontStr = text.substr(begin, i - begin);
            if (colorMap.find(fontStr) == colorMap.end()) {
                std::cerr << "Invalid background font color: " << fontStr << std::endl;
                return -1;
            }
            bColor = colorMap[fontStr];
            
            continue;
        }

        if (i + 1 < text.size() && text[i] == '\\' && text[i + 1] != '\\') {
            if (text[i + 1] == 'n') {
                y++; // line feed
            }
            i += 2;
            x = x0; // carriage return
        }

        x++;
        if (text[i] == ' ') {
            if (this->draw(renderer, "space", x, y, fColor, bColor) == -1)
                return -1;
        } else {
            if (this->draw(renderer, std::string(1, text[i]), x, y, fColor, bColor) == -1)
                return -1;
        }
    }

    return 0;
}