#include "Font.h"
#include "Texture.h"
#include <iostream>
#include <stdexcept>

void Font::setFontColor(Color c) {
    SDL_SetTextureColorMod(mTexture.getTexture(), c.r, c.g, c.b);
    SDL_SetTextureAlphaMod(mTexture.getTexture(), c.a);
}

int Font::draw(const std::string &character, int x, int y, Color fColor, Color bColor)
{
    std::tuple<int, int> position;

    try {
        position = characters.at(character);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of range error: " << oor.what() << std::endl;
        std::cerr << "Invalid rendering token: " << character << std::endl;
        return -1;
    }

    setFontColor(fColor);

    int cellX = std::get<0>(position);
    int cellY = std::get<1>(position);

    SDL_Rect srcRect = { cellX * mCellWidth, cellY * mCellHeight, mCellWidth, mCellHeight };
    SDL_Rect destRect = { x * mCellWidth, y * mCellHeight, mCellWidth, mCellHeight };

    SDL_SetRenderDrawColor(mRenderer, bColor.r, bColor.g, bColor.b, bColor.a);
    SDL_RenderFillRect(mRenderer, &destRect);

    mTexture.render(&srcRect, &destRect);
    return 0;
}

int Font::drawText(const std::string &text, int x0, int y) {
    return drawText(text, x0, y, -1);
}

int Font::drawText(const std::string &text, int x0, int y, Color fColor, Color bColor)
{
    int x = x0;

    for (std::string::size_type i = 0; i < text.size(); ++i) {
        // parse extended token
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            int begin = (int)i + 1;
            while (text[++i] != ')');

            if (this->draw(text.substr(begin, i - begin), x, y, fColor, bColor) == -1)
                return -1;
            x++;
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

int Font::drawText(const std::string &text, int x0, int y, int alpha) {
    Color fColor = Color(0xFF, 0xFF, 0xFF, alpha == -1 ? 0xFF : static_cast<Uint8>(alpha));
    Color bColor = Color(0, 0, 0, 0);
    int x = x0;

    for (std::string::size_type i = 0; i < text.size(); ++i) {
        // parse extended token
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            int begin = (int)i + 1;
            while (text[++i] != ')');

            if (this->draw(text.substr(begin, i - begin), x, y, fColor, bColor) == -1)
                return -1;
            x++;
            continue;
        }

        // parse foreground color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '[') {
            ++i;
            int begin = (int)i + 1;
            while (text[++i] != ']');

            std::string fontStr = text.substr(begin, i - begin);
            if (FontColor::getColorMap().find(fontStr) == FontColor::getColorMap().end()) {
                std::cerr << "Invalid foreground font color: " << fontStr << std::endl;
                return -1;
            }
            fColor = FontColor::getColorMap()[fontStr];
            if (alpha != -1)
                fColor.a = (Uint8)alpha;

            continue;
        }

        // parse background color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '{') {
            ++i;
            int begin = (int)i + 1;
            while (text[++i] != '}');

            std::string fontStr = text.substr(begin, i - begin);
            if (FontColor::getColorMap().find(fontStr) == FontColor::getColorMap().end()) {
                std::cerr << "Invalid background font color: " << fontStr << std::endl;
                return -1;
            }
            bColor = FontColor::getColorMap()[fontStr];

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

int Font::drawText(const std::string &text, int x0, int y, Color bColor) {
    Color fColor = Color(0xFF, 0xFF, 0xFF);
    int x = x0;

    for (std::string::size_type i = 0; i < text.size(); ++i) {
        // parse extended token
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            int begin = (int)i + 1;
            while (text[++i] != ')');

            if (this->draw(text.substr(begin, i - begin), x, y, fColor, bColor) == -1)
                return -1;
            x++;
            continue;
        }

        // parse foreground color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '[') {
            ++i;
            int begin = (int)i + 1;
            while (text[++i] != ']');

            std::string fontStr = text.substr(begin, i - begin);
            if (FontColor::getColorMap().find(fontStr) == FontColor::getColorMap().end()) {
                std::cerr << "Invalid foreground font color: " << fontStr << std::endl;
                return -1;
            }
            fColor = FontColor::getColorMap()[fontStr];

            continue;
        }

        // ignore background color
        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '{') {
            ++i;
            while (text[++i] != '}');
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

int Font::getCellWidth() const {
    return mCellWidth;
}

int Font::getCellHeight() const {
    return mCellHeight;
}

Point Font::getCellSize() const {
    return {mCellWidth, mCellHeight};
}

int getFontStringLength(const std::string& text)
{
    int characters = 0;
    for (std::string::size_type i = 0; i < text.length(); ++i) {
        if (i + 2 < text.size() && text[i] == '$' && text[i + 1] == '(') {
            ++i;
            while (text[++i] != ')');
            characters++;
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
