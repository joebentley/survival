#include "Game.h"
#include "utils.h"
#include "Font.h"
#include <deque>

int Game::init() {
    srand(static_cast<unsigned int>(time(NULL)));

    Uint32 imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    return mSDLManager->initialize(SDL_INIT_VIDEO, imgFlags);
}

int Game::run() {
    mFontTexture = std::make_unique<Texture>(mSDLManager->getRenderer());
    mFontTexture->loadFromFile("resources/curses_800x600.bmp");

    return loop();
}

int Game::loop() {
    bool quit = false;
    SDL_Event e;

    float totalTime = 0;
    std::deque<float> frameTimes;
    float fps = 60;

    SDL_Renderer *renderer = mSDLManager->getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    Font font(mFontTexture.get(), CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, renderer);

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
