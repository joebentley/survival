#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <deque>

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
    SDL_Texture *nightFadeTexture = nullptr;

    Texture texture;
    auto world = std::make_unique<World>();
    world->randomizeWorld();
    
    printf("video mode: %d x %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else if (IMG_Init(imgFlags) != imgFlags) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    } else {
        window = SDL_CreateWindow("Survival",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else {            
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            LightMapTexture lightMapTexture(renderer);

            if (renderer == NULL) {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
            }
            else {
                // if (Texture::LoadFromFile(texture, renderer, "Anikki_square_20x20.bmp") == -1) {
                // if (Texture::LoadFromFile(texture, renderer, "curses_640x300.bmp") == -1) {
                if (Texture::LoadFromFile(texture, renderer, "resources/curses_800x600.bmp") == -1 || lightMapTexture.load() == -1) {
                    printf("Could not load texture!\n");
                    return -1;
                }

                lightMapTexture.init();

                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

                Font font(texture, CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, renderer);
                auto &manager = EntityManager::getInstance();

                auto player = std::make_shared<PlayerEntity>();
                // Place player in center of world
                player->setPos(SCREEN_WIDTH * (WORLD_WIDTH + 1) / 2, SCREEN_HEIGHT * WORLD_HEIGHT / 2);
                manager.addEntity(player);

                auto cat = std::make_shared<CatEntity>("cat1");
                cat->setPos(player->getPos().x - 10, player->getPos().y - 10);
                manager.addEntity(cat);

                auto apple = std::make_shared<AppleEntity>("apple");
                auto banana = std::make_shared<BananaEntity>("banana");
                manager.addEntity(apple);
                manager.addEntity(banana);

                auto pileOfLead = std::make_shared<Entity>("pileOfLead", "Huge Pile Of Lead", "$[grey]L");
                pileOfLead->addBehaviour(std::make_shared<PickuppableBehaviour>(*pileOfLead, 100));
                manager.addEntity(pileOfLead);

                apple->setPos(player->getPos() + Point(2, 2));
                banana->setPos(player->getPos() + Point(3, 2));
                pileOfLead->setPos(player->getPos() + Point(2, 2));

                auto chest = std::make_shared<ChestEntity>("chest");
                chest->setPos(player->getPos() + Point(-2, 2));
                manager.addEntity(chest);

                chest->addToInventory(std::make_shared<AppleEntity>());

                auto statusUI = std::make_shared<StatusUIEntity>(dynamic_cast<PlayerEntity&>(*player));
                manager.addEntity(statusUI);

                auto fire = std::make_shared<FireEntity>();
                fire->setPos(player->getPos() + Point(0, -2));
                manager.addEntity(fire);

                player->Entity::addToInventory(std::make_shared<TwigEntity>());
                player->Entity::addToInventory(std::make_shared<TwigEntity>());
                player->Entity::addToInventory(std::make_shared<GrassTuftEntity>());
                player->Entity::addToInventory(std::make_shared<GrassTuftEntity>());
                player->Entity::addToInventory(std::make_shared<GrassTuftEntity>());
                player->Entity::addToInventory(std::make_shared<GrassTuftEntity>());

                player->equip(EquipmentSlot::RIGHT_HAND, std::make_shared<TorchEntity>());

                manager.initialize();
                manager.setTimeOfDay(Time(4, 0));

                // TODO: Tidy up all the screens somehow?
                InventoryScreen inventoryScreen(*player);
                LootingDialog lootingDialog(*player);
                InspectionDialog inspectionDialog(*player);
                CraftingScreen craftingScreen(*player);
                EquipmentScreen equipmentScreen(*player);

                bool quit = false;
                SDL_Event e;

                float totalTime = 0;
                std::deque<float> frameTimes;

                while (!quit) {
                    // TODO: Fix random slow input frames
                    while (SDL_PollEvent(&e) != 0) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                        else if (e.type == SDL_KEYDOWN) {
                            if (inventoryScreen.enabled)
                                inventoryScreen.handleInput(e.key);
                            else if (lootingDialog.enabled)
                                lootingDialog.handleInput(e.key);
                            else if (inspectionDialog.enabled)
                                inspectionDialog.handleInput(e.key);
                            else if (craftingScreen.enabled)
                                craftingScreen.handleInput(e.key);
                            else if (equipmentScreen.enabled)
                                equipmentScreen.handleInput(e.key);
                            else
                                dynamic_cast<PlayerEntity &>(*player).handleInput(e.key, quit, inventoryScreen,
                                                                                  lootingDialog, inspectionDialog,
                                                                                  craftingScreen, equipmentScreen);
                        }
                    }

                    beginTime();

                    manager.cleanup();

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
                    SDL_RenderClear(renderer);

                    if (inventoryScreen.enabled)
                        inventoryScreen.render(font);
                    else if (craftingScreen.enabled)
                        craftingScreen.render(font, *world, lightMapTexture);
                    else if (equipmentScreen.enabled)
                        equipmentScreen.render(font);
                    else if (!lootingDialog.viewingDescription && !inspectionDialog.viewingDescription) {
                        if (world->render(font, player->getWorldPos()) == -1)
                            return -1;
                        manager.render(font, player->getWorldPos(), lightMapTexture);

                        // Always render UI
                        statusUI->render(font, player->getWorldPos());
                    }

                    if (lootingDialog.enabled)
                        lootingDialog.render(font);

                    if (inspectionDialog.enabled)
                        inspectionDialog.render(font);

                    if (player->hp <= 0) {
                        showMessageBox(font, "$[red]You died!", 10, 10);
                        font.drawText("${black}press return to quit!", 20, 20);
                    }

                    totalTime += endTime();
                    frameTimes.push_back(endTime());

                    if (frameTimes.size() > 60) {
                        totalTime -= frameTimes.front();
                        frameTimes.pop_front();
                    }

                    float fps = frameTimes.size() / totalTime;


                    font.drawText(std::to_string(fps), SCREEN_WIDTH - 5, SCREEN_HEIGHT - 1);

                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyTexture(nightFadeTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}