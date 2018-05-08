#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

#include "texture.h"
#include "font.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const std::string CHARS =
    "space dwarf dwarf2 heart diamond club spade circle emptycircle ring emptyring male female note1 note2 gem "
    "sloperight slopeleft updown alert pagemark sectionmark thickbottom updown2 up down right left misc leftright slopeup slopedown "
    "space2 ! \" # $ % & ' ( ) * + , - . / "
    "0 1 2 3 4 5 6 7 8 9 : ; < = > ? "
    "@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _ "
    "` a b c d e f g h i j k l m n o p q r s t u v w x y z { : } ~ triangle";

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

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

                bool quit = false;
                SDL_Event e;

                while (!quit) {
                    while (SDL_PollEvent(&e) != 0) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                    }

                    SDL_RenderClear(renderer);
                    if (font.drawText(renderer, "hello$(dwarf2)WORLD$[yellow]dwarf\\nhello", 2, 2) == -1)
                        return -1;
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}