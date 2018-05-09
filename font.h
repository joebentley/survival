
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

const int CHAR_HEIGHT = 12; // 20; // 12;
const int CHAR_WIDTH = 10; // 20; // 8;
const int NUM_PER_ROW = 16;
const std::string CHARS =
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
    "equiv pm gteq lteq upperint lowerint div approx degree cdot hyphen sqrt endquote power2 block space3";


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