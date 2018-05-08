#include <SDL2/SDL.h>
#include <stdio.h>

#include "texture.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[])
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
                    // SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );
                    texture.render(renderer, NULL, NULL);
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}