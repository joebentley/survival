#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

#include "texture.h"
#include "font.h"
#include "dialog.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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

int main(int argc, char* argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    Texture texture;
    

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else {
        window = SDL_CreateWindow("Survival",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL) {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            }
            else {
                if (Texture::LoadFromFile(texture, renderer, "curses_640x300.bmp") == -1) {
                    printf("Could not load texture!\n");
                    return -1;
                }

                Font font(texture, 8, 12, 16, CHARS);

                bool quit = false;
                SDL_Event e;

                while (!quit) {
                    while (SDL_PollEvent(&e) != 0) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                    }

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
                    SDL_RenderClear(renderer);
                    showMessageBox(renderer, font, "Hello world!", 4, 4);
                    // if (font.drawText(renderer, "hello$(dwarf2)WORLD$[yellow]dwarf\\nhello$(block)${yellow}$[grey]Hello", 2, 2) == -1)
                    //     return -1;
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}