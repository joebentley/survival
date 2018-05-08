#include "font.h"
#include <iostream>

enum FontColor fontColorFromString(const std::string &colorStr) {
    if (colorStr == "white") {
        return FONT_WHITE;
    } else if (colorStr == "yellow") {
        return FONT_YELLOW;
    } else if (colorStr == "grey") {
        return FONT_GREY;
    } else if (colorStr == "transparent") {
        return FONT_TRANSPARENT;
    }

    return FONT_INVALID;
}

struct Color colorFromFontColor(enum FontColor color) {
    if (color == FONT_WHITE) {
        return (struct Color) { 0xFF, 0xFF, 0xFF, 0xFF };
    } else if (color == FONT_YELLOW) {
        return (struct Color) { 0xFF, 0xFF, 0, 0xFF };
    } else if (color == FONT_GREY) {
        return (struct Color) { 0x55, 0x55, 0x55, 0xFF };
    } else if (color == FONT_TRANSPARENT) {
        return (struct Color) { 0, 0, 0, 0 };
    }
    return (struct Color) { 0xFF, 0xFF, 0xFF, 0xFF };
}

void Font::setFontColor(enum FontColor color) {
    struct Color c = colorFromFontColor(color);
    SDL_SetTextureColorMod(texture.texture, c.r, c.g, c.b);
}

int Font::draw(SDL_Renderer *renderer, const std::string &character, int x, int y, enum FontColor fColor, enum FontColor bColor)
{
    setFontColor(fColor);

    for (int i = 0; i < characters.size(); ++i) {
        if (characters[i] == character) {
            int cellX = i % numPerRow;
            int cellY = i / numPerRow;

            SDL_Rect srcRect = { cellX * cellWidth, cellY * cellHeight, cellWidth, cellHeight };
            SDL_Rect destRect = { x * cellWidth, y * cellHeight, cellWidth, cellHeight };

            if (bColor != FONT_TRANSPARENT) {
                struct Color bc = colorFromFontColor(bColor);
                SDL_SetRenderDrawColor(renderer, bc.r, bc.g, bc.b, bc.a);
                SDL_RenderFillRect(renderer, &destRect);
            }

            texture.render(renderer, &srcRect, &destRect);
            return 0;
        }
    }

    std::cerr << "Invalid rendering token: " << character << std::endl;
    return -1;
}

int Font::drawText(SDL_Renderer *renderer, const std::string &text, int x0, int y)
{
    enum FontColor fColor = FONT_WHITE;
    enum FontColor bColor = FONT_TRANSPARENT;
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
            fColor = fontColorFromString(fontStr);
            if (fColor == FONT_INVALID) {
                std::cerr << "Invalid foreground font color: " << fontStr << std::endl;
                return -1;
            }
            
            continue;
        }

        // parse background color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '{') {
            ++i;
            int begin = i + 1;
            while (text[++i] != '}');

            std::string fontStr = text.substr(begin, i - begin);
            bColor = fontColorFromString(fontStr);
            if (bColor == FONT_INVALID) {
                std::cerr << "Invalid background font color: " << fontStr << std::endl;
                return -1;
            }
            
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