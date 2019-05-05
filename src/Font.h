
#ifndef FONT_H_
#define FONT_H_

#include "Point.h"
#include "FontColor.h"
#include "Texture.h"

#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <tuple>

/// Height in pixels of each character on the character map
const int CHAR_HEIGHT = 12; // 20; // 12;
/// Width in pixels of each char
const int CHAR_WIDTH = 10; // 20; // 8;
/// Number of rows on the character map
const int NUM_PER_ROW = 16;
/// Strings that should be associated with each character on the character map in order of appearance
const std::string CHARS = // NOLINT(cert-err58-cpp)
    "space dwarf dwarf2 heart diamond club spade circle emptycircle ring emptyring male female note1 note2 gem "
    "sloperight slopeleft updown alert pagemark sectionmark thickbottom updown2 up down right left boxbottomleft leftright slopeup slopedown "
    "space2 ! \" # $ % & ' ( ) * + , - . / "
    "0 1 2 3 4 5 6 7 8 9 : ; < = > ? "
    "@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _ "
    "` a b c d e f g h i j k l m n o p q r s t u v w x y z { : } ~ triangle "
    "accentC accentu accente accenta accenta2 accenta3 accenta4 accentc accente2 accente3 accente4 accenti accenti2 accenti3 accentA  accentA2 "
    "accentE accentae accentAE accento accento2 accento3 accentu2 accentu3 accenty accentO accentU cent pound yen Pt function "
    "accenta5 accenti4 accento4 accentu4 accentn accentN aoverbar ooverbar qmark2 boxtopleft boxtopright half quarter emark2 muchless muchgreater "
    "shaded shaded2 shaded3 p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 "
    "p14 p15 p16 p17 p18 p19 p20 p21 p22 p23 p24 p25 p26 p27 p28 p29 "
    "p30 p31 p32 p33 p34 p35 p36 p37 p38 p39 p40 p41 p42 p43 p44 p45 "
    "alpha beta Gamma Pi Sigma sigma mu tau Phi theta Omega delta inf ninf in intersect "
    "equiv pm gteq lteq upperint lowerint div approx degree cdot hyphen sqrt endquote power2 block space3 "
    "bunny1 bunny2";


/// This class describes a bitmap font texture coupled with an SDL_Renderer, allowing the drawing of complex colored text
/// and symbols to the screen at any position
class Font {
    /// The entire font texture
    Texture& mTexture;
    /// Width of each font cell in pixels
    int mCellWidth;
    /// Height of each font cell in pixels
    int mCellHeight;
    /// SDL_Renderer instance to render to
    SDL_Renderer *mRenderer;
public:
    /// Map from each character's string representation (as in CHARS) to an (x, y) tuple of grid position within the font
    std::unordered_map<std::string, std::tuple<int, int>> characters;

    /// Initialize a new font from the given texture, width of cell, height of cell, number of cells per row,
    /// whitespace-separated characters in order of appearance, and SDL_Renderer to render to
    Font(Texture &texture, int cellWidth, int cellHeight, int numPerRow,
         const std::string &characters, SDL_Renderer *renderer)
        : mTexture(texture), mCellWidth(cellWidth),
          mCellHeight(cellHeight), mRenderer(renderer)
    {
        // Separate characters string by whitespace
        std::vector<std::string> words;
        std::istringstream iss(characters);
        std::copy(std::istream_iterator<std::string>(iss),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(words));

        // Generate the map of character coordinate tuple pairs
        for (unsigned long i = 0; i < words.size(); ++i) {
            int x = (int)i % numPerRow;
            int y = (int)i / numPerRow;

            this->characters[words[i]] = std::make_tuple(x, y);
        }
    }

    /// Set the font color via the underlying texture's color and alpha mod
    void setFontColor(Color color);
    int draw(const std::string &character, int x, int y) {
        return draw(character, x, y, Color(0xFF, 0xFF, 0xFF, 0xFF), Color(0, 0, 0, 0));
    }
    int draw(const std::string &character, Point p) {
        return draw(character, p.mX, p.mY);
    }
    int draw(const std::string &character, int x, int y, Color fColor) {
        return draw(character, x, y, fColor, Color(0, 0, 0, 0));
    }
    int draw(const std::string &character, Point p, Color fColor) {
        return draw(character, p.mX, p.mY, fColor);
    }
    /// Draw character given by `character` onto the screen at grid coordinates (x, y) with given foreground and background colors
    int draw(const std::string &character, int x, int y, Color fColor, Color bColor);
    int draw(const std::string &character, Point p, Color fColor, Color bColor) {
        return draw(character, p.mX, p.mY, fColor, bColor);
    }
    int drawText(const std::string &text, int x, int y);
    int drawText(const std::string &text, Point p) {
        return drawText(text, p.mX, p.mY);
    }
    /// Draw a string of characters onto the screen with multicharacter elements given in $(element)
    int drawText(const std::string &text, int x, int y, Color fColor, Color bColor);
    int drawText(const std::string &text, Point p, Color fColor, Color bColor) {
        return drawText(text, p.mX, p.mY, fColor, bColor);
    }
    // Only use the background color, use other colors from text
    int drawText(const std::string &text, int x, int y, Color bColor);

    /// Draws the text on the screen
    /// \param text fontstring text to be rendered, $[color] specifies a foreground color and ${color} for background (see FontColor.h)
    /// \param x x-coord cell to render in
    /// \param y y-coord cell to render in
    /// \param alpha transparency to render foreground color with, use -1 for no transparency
    /// \return status code
    int drawText(const std::string &text, int x, int y, int alpha);

    /// Get cell width
    int getCellWidth() const;
    /// Get cell height
    int getCellHeight() const;
    /// Get cell dimensions as Point (w, h)
    Point getCellSize() const;
};

/// Get number of actual characters in the font string (disregarding $(), $[], ${})
int getFontStringLength(const std::string& string);

#endif // FONT_H_
