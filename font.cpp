#include "font.h"
#include <iostream>

std::unordered_map<std::string, Color> colorMap = {
    {"white", (struct Color) { 0xFF, 0xFF, 0xFF, 0xFF }},
    {"yellow", (struct Color) { 0xFF, 0xFF, 0, 0xFF }},
    {"grey", (struct Color) { 0x88, 0x88, 0x88, 0xFF }},
    {"transparent", (struct Color) { 0, 0, 0, 0 }}
};

Color getColor(const std::string& colorStr) {
    return colorMap[colorStr];    
}

void Font::setFontColor(Color c) {
    SDL_SetTextureColorMod(texture.texture, c.r, c.g, c.b);
}

int Font::draw(const std::string &character, int x, int y, Color fColor, Color bColor)
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

int Font::drawText(const std::string &text, int x0, int y)
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
            
            if (this->draw(text.substr(begin, i - begin), x, y, fColor, bColor) == -1)
                return -1;
            x++;
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

        if (text[i] == ' ') {
            if (this->draw("space", x, y, fColor, bColor) == -1)
                return -1;
        } else {
            if (this->draw(std::string(1, text[i]), x, y, fColor, bColor) == -1)
                return -1;
        }
        x++;        
    }

    return 0;
}

int getFontStringLength(const std::string& text)
{
    int characters = 0;
    for (int i = 0; i < text.length(); ++i) {
        if (i + 2 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            while (text[++i] != ')');
            continue;
        }
        if (i + 2 < text.size() && text[i] == '$' && text[i + 1] == '[') {
            ++i;
            while (text[++i] != ']');
            continue;
        }
        if (i + 2 < text.size() && text[i] == '$' && text[i + 1] == '{') {
            ++i;
            while (text[++i] != '}');
            continue;
        }
        if (i + 1 < text.size() && text[i] == '\\' && text[i + 1] != '\\') {
            i += 2;
            continue;
        }
        if (i + 1 < text.size() && text[i] == '\\' && text[i + 1] == '\\') {
            i += 2;
            characters++;
            continue;
        }
        characters++;
    }

    return characters;
}
