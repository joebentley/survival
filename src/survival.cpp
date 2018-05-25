
//#ifdef _MSC_VER
//#include <SDL_image.h>
//#include <SDL.h>
//#else
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL.h>
//#endif

//#include <string>
//#include <memory>
//#include <ctime>
//#include <cstdlib>
//#include <cstdio>
//#include <deque>
//#include "Texture.h"
//#include "Font.h"
//#include "UI.h"
//#include "entity.h"
//#include "world.h"
//#include "entities.h"

#include "Game.h"

//const int MAX_FRAME_RATE = 30;

int main(int argc, char* argv[])
{
    Game game;
    if (game.init() != 0)
        return -1;
    else
        game.run();
    return 0;
//    srand(static_cast<unsigned int>(time(NULL)));
//
//    SDL_Window *window = nullptr;
//    SDL_Renderer *renderer = nullptr;
//    SDL_Texture *nightFadeTexture = nullptr;
//
//    Texture texture;
//	World world;
//
//    printf("video mode: %d x %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
//
//    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
//
//    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
//    } else if (IMG_Init(imgFlags) != imgFlags) {
//        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
//    } else {
//        window = SDL_CreateWindow("Survival",
//            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
//
//        if (window == NULL) {
//            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//        }
//        else {
//            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
//            LightMapTexture lightMapTexture(renderer);
//
//            if (renderer == NULL) {
//                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
//            }
//            else {
//                if (Texture::loadFromFile(renderer) == -1 || lightMapTexture.load() == -1) {
//                    printf("Could not load texture!\n");
//                    return -1;
//                }
//
//                lightMapTexture.init();
//
//                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//
//                Font font(texture, CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, renderer);
//                auto &manager = EntityManager::getInstance();
//
//                // TODO clean up all this initialisation stuff
//                auto pPlayer = makeEntity<PlayerEntity>();
//                // Place player in center of world
//                // TODO: Fix bug that occurs at (0,0)
//                pPlayer->setPos(SCREEN_WIDTH * 1000 + SCREEN_WIDTH / 2, SCREEN_HEIGHT * 1000 + SCREEN_HEIGHT / 2);
//                auto playerPos = pPlayer->getPos();
//
//                makeEntity<CatEntity>()->setPos(playerPos.mX - 10, playerPos.mY - 10);
//
//                auto apple = makeEntity<AppleEntity>();
//                auto banana = makeEntity<BananaEntity>();
//                apple->setPos(playerPos + Point(2, 2));
//                banana->setPos(playerPos + Point(3, 2));
//
//                auto pileOfLead = std::make_unique<Entity>("pileOfLead", "Huge Pile Of Lead", "$[grey]L");
//                pileOfLead->addBehaviour(std::make_unique<PickuppableBehaviour>(*pileOfLead, 100));
//                pileOfLead->setPos(playerPos + Point(2, 2));
//                manager.addEntity(std::move(pileOfLead));
//
//                auto chest = std::make_unique<ChestEntity>("chest");
//                auto pChest = chest.get();
//                chest->setPos(playerPos + Point(-2, 2));
//                manager.addEntity(std::move(chest));
//
//                makeEntityAndAddToInventory<AppleEntity>(pChest);
//
//                auto statusUI = std::make_unique<StatusUIEntity>(dynamic_cast<PlayerEntity&>(*pPlayer));
//                auto pStatusUI = statusUI.get();
//                manager.addEntity(std::move(statusUI));
//
//                makeEntityAndAddToInventory<WaterskinEntity>(pPlayer);
//                makeEntityAndAddToInventory<GrassTuftEntity>(pPlayer);
//                makeEntityAndAddToInventory<GrassTuftEntity>(pPlayer);
//                makeEntityAndAddToInventory<TwigEntity>(pPlayer);
//                makeEntityAndAddToInventory<TwigEntity>(pPlayer);
//                makeEntityAndAddToInventory<TwigEntity>(pPlayer);
//
//                manager.initialize();
//                manager.setTimeOfDay(Time(6, 0));
//
//                // TODO change to unique_ptr
//                std::unordered_map<ScreenType, std::shared_ptr<Screen>> screens;
//
//                screens[ScreenType::NOTIFICATION] = std::make_shared<NotificationMessageScreen>();
//                screens[ScreenType::INVENTORY] = std::make_shared<InventoryScreen>(*pPlayer);
//                screens[ScreenType::LOOTING] = std::make_shared<LootingDialog>(*pPlayer);
//                screens[ScreenType::INSPECTION] = std::make_shared<InspectionDialog>(*pPlayer);
//                screens[ScreenType::CRAFTING] = std::make_shared<CraftingScreen>(*pPlayer);
//                screens[ScreenType::EQUIPMENT] = std::make_shared<EquipmentScreen>(*pPlayer);
//                screens[ScreenType::HELP] = std::make_shared<HelpScreen>();
//
//                bool initialMessage = true;
//                std::vector<std::string> initialMessageLines({
//                        "Welcome to the game",
//                        "? for help (once you've closed this)",
//                        "return to start"
//                });
//
//                bool quit = false;
//                SDL_Event e;
//
//                float totalTime = 0;
//                std::deque<float> frameTimes;
//                float fps = 60;
//
//                while (!quit) {
//                    beginTime();
//
//                    // TODO: Fix random slow input frames
//                    while (SDL_PollEvent(&e) != 0) {
//                        if (e.type == SDL_QUIT)
//                            quit = true;
//                        else if (initialMessage && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
//                            initialMessage = false;
//                        else if (!initialMessage && e.type == SDL_KEYDOWN) {
//                            bool screenEnabled = false;
//                            for (auto screen : screens) {
//                                if (screen.second->isEnabled()) {
//                                    screen.second->handleInput(e.key);
//                                    screenEnabled = true;
//                                    break;
//                                }
//                            }
//                            if (!screenEnabled)
//                                dynamic_cast<PlayerEntity &>(*pPlayer).handleInput(e.key, quit, screens);
//                        }
//                    }
//
//                    manager.cleanup();
//
//                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
//                    SDL_RenderClear(renderer);
//
//                    bool shouldRenderWorld = true;
//                    std::shared_ptr<Screen> screenToRender = nullptr;
//                    for (auto screen : screens) {
//                        if (screen.second->isEnabled()) {
//                            shouldRenderWorld = screen.second->shouldRenderWorld();
//                            screenToRender = screen.second;
//                            break;
//                        }
//                    }
//
//                    if (shouldRenderWorld) {
//                        world.render(font, pPlayer->getWorldPos());
//                        manager.render(font, pPlayer->getWorldPos(), lightMapTexture);
//                    }
//
//                    // Always render status and notification UI
//                    pStatusUI->render(font, pPlayer->getWorldPos());
//                    NotificationMessageRenderer::getInstance().render(font);
//
//                    if (screenToRender != nullptr) {
//                        screenToRender->render(font);
//                    }
//
//                    if (pPlayer->mHp <= 0) {
//                        MessageBoxRenderer::getInstance().queueMessageBoxCentered(
//                                std::vector<std::string> {"$[red]You died!", "", "Press return to quit"}, 1);
//                    }
//
//                    if (initialMessage)
//                        MessageBoxRenderer::getInstance().queueMessageBoxCentered(initialMessageLines, 1);
//
//                    MessageBoxRenderer::getInstance().render(font);
//
//                    font.drawText(std::to_string(fps), SCREEN_WIDTH - 5, SCREEN_HEIGHT - 1);
//
//                    SDL_RenderPresent(renderer);
//
//                    // Cap framerate
//                    auto frameTimeBeforeCap = endTime();
//
//                    auto secondsTooFastBy = 1.0/MAX_FRAME_RATE - frameTimeBeforeCap;
//                    if (secondsTooFastBy > 0) {
//                        SDL_Delay(static_cast<Uint32>(secondsTooFastBy * 1000));
//                    }
//
//                    frameTimes.push_back(static_cast<float>(frameTimeBeforeCap + (secondsTooFastBy > 0 ? secondsTooFastBy : 0)));
//                    totalTime += frameTimes.back();
//
//                    if (frameTimes.size() > 60) {
//                        totalTime -= frameTimes.front();
//                        frameTimes.pop_front();
//                    }
//
//                    fps = frameTimes.size() / totalTime;
//                }
//            }
//        }
//    }
//
//    SDL_DestroyTexture(nightFadeTexture);
//    SDL_DestroyWindow(window);
//    SDL_Quit();
//    return 0;
}