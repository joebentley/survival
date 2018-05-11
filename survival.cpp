#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <cstdio>

#include "texture.h"
#include "font.h"
#include "dialog.h"
#include "entity.h"
#include "world.h"
#include "entities.h"

//Screen dimension constants
const int WINDOW_WIDTH = CHAR_WIDTH * SCREEN_WIDTH;
const int WINDOW_HEIGHT = CHAR_HEIGHT * SCREEN_HEIGHT;

int main(int argc, char* argv[])
{
    srand(time(NULL));
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    Texture texture;
    auto world = std::make_unique<World>();
    world->randomizeFloor();
    
    printf("video mode: %d x %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else {
        window = SDL_CreateWindow("Survival",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else {            
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL) {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            }
            else {
                // if (Texture::LoadFromFile(texture, renderer, "Anikki_square_20x20.bmp") == -1) {
                // if (Texture::LoadFromFile(texture, renderer, "curses_640x300.bmp") == -1) {
                if (Texture::LoadFromFile(texture, renderer, "resources/curses_800x600.bmp") == -1) {
                    printf("Could not load texture!\n");
                    return -1;
                }

                // SDL_RenderSetScale(renderer, 1.5, 1.5);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

                Font font(texture, CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, renderer);
                EntityManager manager;

                auto player = std::make_shared<PlayerEntity>(manager);
                // Place player in center of world
                player->setPos(SCREEN_WIDTH * (SCREEN_WIDTH + 1) / 2, SCREEN_HEIGHT * SCREEN_HEIGHT / 2);
                manager.addEntity(player);

                auto cat = std::make_shared<CatEntity>(manager, "cat1");
                cat->setPos(player->pos.x - 10, player->pos.y - 10);
                manager.addEntity(cat);

                auto apple = std::make_shared<Entity>(manager, "apple1", "food", "Apple", "$[green]a");
                apple->shortDesc = "A small, fist-sized fruit that is hopefully crispy and juicy";
                apple->longDesc = "This is a longer description of the apple";
                apple->canBePickedUp = true;
                manager.addEntity(apple);

                auto banana = std::make_shared<Entity>(manager, "banana1", "food", "Banana", "$[yellow]b");
                banana->shortDesc = "A yellow fruit found in the jungle. The shape looks familiar...";
                banana->longDesc = "This fruit was discovered in [redacted]. They were brought west by Arab conquerors in 327 B.C.";
                banana->canBePickedUp = true;
                manager.addEntity(banana);

                player->addToInventory(apple);
                player->addToInventory(banana);

                std::shared_ptr<Entity> healthUI = std::make_shared<StatusUIEntity>(manager, dynamic_cast<PlayerEntity&>(*player));
                manager.addEntity(healthUI);

                manager.initialize();

                InventoryScreen inventoryScreen(*player);

                bool quit = false;
                SDL_Event e;

                while (!quit) {
                    while (SDL_PollEvent(&e) != 0) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN) {
                            if (inventoryScreen.enabled)
                                inventoryScreen.handleInput(e.key);
                            else
                                dynamic_cast<PlayerEntity&>(*player).handleInput(e.key, quit, inventoryScreen);
                        }
                    }

                    manager.cleanup();

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
                    SDL_RenderClear(renderer);

                    if (inventoryScreen.enabled)
                        inventoryScreen.render(font);
                    else {
                        if (world->render(font, player->getWorldPos()) == -1)
                            return -1;
                        manager.render(font, player->getWorldPos());
                    }

                    if (player->hp <= 0) {
                        showMessageBox(font, "$[red]You died!", 10, 10);
                        font.drawText("${black}press return to quit!", 20, 20);
                    }

                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}