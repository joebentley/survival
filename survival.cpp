#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <memory>

#include "texture.h"
#include "font.h"
#include "dialog.h"
#include "entity.h"
#include "world.h"
#include "behaviours.h"

//Screen dimension constants
const int WINDOW_WIDTH = CHAR_WIDTH * SCREEN_WIDTH;
const int WINDOW_HEIGHT = CHAR_HEIGHT * SCREEN_HEIGHT;

int main(int argc, char* argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    Texture texture;
    World world;
    world.randomizeFloor();
    
    printf("video mode: %d x %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else {
        window = SDL_CreateWindow("Survival",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            2*WINDOW_WIDTH, 2*WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

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

                // SDL_RenderSetScale(renderer, 1, 1);

                Font font(texture, CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, renderer);
                EntityManager manager;
                Entity player("player", "$[white]$(dwarf)", manager);
                player.setPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                PlayerInputBehaviour playerInputBehaviour(player);
                player.addBehaviour(&playerInputBehaviour);
                
                manager.addEntity(player);
                manager.initialize();

                bool quit = false;
                SDL_Event e;

                while (!quit) {
                    while (SDL_PollEvent(&e) != 0) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_h:
                                    manager.broadcast("input left");
                                    break;
                                case SDLK_j:                      
                                    manager.broadcast("input down");
                                    break;
                                case SDLK_k:
                                    manager.broadcast("input up");
                                    break;
                                case SDLK_l:
                                    manager.broadcast("input right");
                                    break;
                                case SDLK_y:
                                    manager.broadcast("input upleft");
                                    break;
                                case SDLK_u:                      
                                    manager.broadcast("input upright");
                                    break;
                                case SDLK_b:
                                    manager.broadcast("input downleft");
                                    break;
                                case SDLK_n:
                                    manager.broadcast("input downright");
                                    break;
                            }
                        }
                    }

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
                    SDL_RenderClear(renderer);
                    if (world.render(font) == -1)
                        return -1;
                    player.render(font);
                    // showMessageBox(font, "$[yellow]Hello world!", 4, 4);
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