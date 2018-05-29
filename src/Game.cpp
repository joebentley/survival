#include "Game.h"
#include "utils.h"
#include "Font.h"
#include "Entity.h"
#include "Entities.h"
#include "EntityBuilder.h"
#include <deque>

int Game::init() {
    srand(static_cast<unsigned int>(time(NULL)));

    Uint32 imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    return mSDLManager.initialize(SDL_INIT_VIDEO, imgFlags);
}

void Game::run() {
    mFontTexture = std::make_unique<Texture>(mSDLManager.getRenderer());
    mFontTexture->loadFromFile("resources/curses_800x600.bmp");

    loop();
}

void Game::loop() {
    SDL_Renderer *renderer = mSDLManager.getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    LightMapTexture lightMapTexture(renderer);

    Font font(*mFontTexture, CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, renderer);

    World world;
    auto &manager = EntityManager::getInstance();

    // TODO clean up all this initialisation stuff
    auto pPlayer = EntityBuilder::makeEntity<PlayerEntity>();
    // Place player in center of world
    // TODO: Fix bug that occurs at (0,0)
    pPlayer->setPos(SCREEN_WIDTH * 1000 + SCREEN_WIDTH / 2, SCREEN_HEIGHT * 1000 + SCREEN_HEIGHT / 2);
    auto playerPos = pPlayer->getPos();

    EntityBuilder::makeEntity<CatEntity>()->setPos(playerPos.mX - 10, playerPos.mY - 10);

    auto apple = EntityBuilder::makeEntity<AppleEntity>();
    auto banana = EntityBuilder::makeEntity<BananaEntity>();
    apple->setPos(playerPos + Point(2, 2));
    banana->setPos(playerPos + Point(3, 2));

    auto pPileOfLead = EntityBuilder::makeEntity("pileOfLead", "Huge Pile Of Lead", "$[grey]L");
    pPileOfLead->addProperty(std::make_unique<PickuppableProperty>(100));
    pPileOfLead->setPos(playerPos + Point(2, 2));

    auto pChest = EntityBuilder::makeEntity<ChestEntity>();
    pChest->setPos(playerPos + Point(-2, 2));

    EntityBuilder::makeEntityAndAddToInventory<AppleEntity>(pChest);

    auto statusUI = std::make_unique<StatusUIEntity>(dynamic_cast<PlayerEntity&>(*pPlayer));
    auto pStatusUI = statusUI.get();
    manager.addEntity(std::move(statusUI));

    EntityBuilder::makeEntityAndAddToInventory<WaterskinEntity>(pPlayer);
    EntityBuilder::makeEntityAndAddToInventory<GrassTuftEntity>(pPlayer);
    EntityBuilder::makeEntityAndAddToInventory<GrassTuftEntity>(pPlayer);
    EntityBuilder::makeEntityAndAddToInventory<TwigEntity>(pPlayer);
    EntityBuilder::makeEntityAndAddToInventory<TwigEntity>(pPlayer);
    EntityBuilder::makeEntityAndAddToInventory<TwigEntity>(pPlayer);

    manager.initialize();
    manager.setTimeOfDay(Time(6, 0));

    // TODO change to unique_ptr
    std::unordered_map<ScreenType, std::shared_ptr<Screen>> screens;

    screens[ScreenType::NOTIFICATION] = std::make_shared<NotificationMessageScreen>();
    screens[ScreenType::INVENTORY] = std::make_shared<InventoryScreen>(*pPlayer);
    screens[ScreenType::LOOTING] = std::make_shared<LootingDialog>(*pPlayer);
    screens[ScreenType::INSPECTION] = std::make_shared<InspectionDialog>(*pPlayer);
    screens[ScreenType::CRAFTING] = std::make_shared<CraftingScreen>(*pPlayer);
    screens[ScreenType::EQUIPMENT] = std::make_shared<EquipmentScreen>(*pPlayer);
    screens[ScreenType::HELP] = std::make_shared<HelpScreen>();

    bool initialMessage = true;
    std::vector<std::string> initialMessageLines({
            "Welcome to the game",
            "? for help (once you've closed this)",
            "return to start"
    });

    bool quit = false;
    SDL_Event e;

    float totalTime = 0;
    std::deque<float> frameTimes;
    float fps = 60;

    while (!quit) {
        beginTime();

        // TODO: Fix random slow input frames
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (initialMessage && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
                initialMessage = false;
            else if (!initialMessage && e.type == SDL_KEYDOWN) {
                bool screenEnabled = false;
                for (auto screen : screens) {
                    if (screen.second->isEnabled()) {
                        screen.second->handleInput(e.key);
                        screenEnabled = true;
                        break;
                    }
                }
                if (!screenEnabled)
                    dynamic_cast<PlayerEntity &>(*pPlayer).handleInput(e.key, quit, screens);
            }
        }

        manager.cleanup();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        bool shouldRenderWorld = true;
        std::shared_ptr<Screen> screenToRender = nullptr;
        for (auto screen : screens) {
            if (screen.second->isEnabled()) {
                shouldRenderWorld = screen.second->shouldRenderWorld();
                screenToRender = screen.second;
                break;
            }
        }

        if (shouldRenderWorld) {
            world.render(font, pPlayer->getWorldPos());
            manager.render(font, pPlayer->getWorldPos(), lightMapTexture);
        }

        // Always render status and notification UI
        pStatusUI->render(font, pPlayer->getWorldPos());
        NotificationMessageRenderer::getInstance().render(font);

        if (screenToRender != nullptr) {
            screenToRender->render(font);
        }

        if (pPlayer->mHp <= 0) {
            MessageBoxRenderer::getInstance().queueMessageBoxCentered(
                    std::vector<std::string> {"$[red]You died!", "", "Press return to quit"}, 1);
        }

        if (initialMessage)
            MessageBoxRenderer::getInstance().queueMessageBoxCentered(initialMessageLines, 1);

        MessageBoxRenderer::getInstance().render(font);

        font.drawText(std::to_string(fps), SCREEN_WIDTH - 5, SCREEN_HEIGHT - 1);

        SDL_RenderPresent(renderer);

        // Cap framerate
        auto frameTimeBeforeCap = endTime();

        auto secondsTooFastBy = 1.0/MAX_FRAME_RATE - frameTimeBeforeCap;
        if (secondsTooFastBy > 0) {
            SDL_Delay(static_cast<Uint32>(secondsTooFastBy * 1000));
        }

        frameTimes.push_back(static_cast<float>(frameTimeBeforeCap + (secondsTooFastBy > 0 ? secondsTooFastBy : 0)));
        totalTime += frameTimes.back();

        if (frameTimes.size() > 60) {
            totalTime -= frameTimes.front();
            frameTimes.pop_front();
        }

        fps = frameTimes.size() / totalTime;
    }
}
