#include "Game.h"
#include "Entity/Building/BuildingWallEntity.h"
#include "Entity/ChestEntity.h"
#include "Entity/Entity.h"
#include "Entity/Items/Food/AppleEntity.h"
#include "Entity/Items/Food/BananaEntity.h"
#include "Entity/Items/Materials/GrassTuftEntity.h"
#include "Entity/Items/Materials/TwigEntity.h"
#include "Entity/Items/WaterskinEntity.h"
#include "Entity/NPCs/CatEntity.h"
#include "EntityBuilder.h"
#include "utils.h"

#include <deque>

Game::Game()
    : mSDLManager(SDL_INIT_VIDEO), m_lightMapTexture(mSDLManager.getRenderer()), mFontTexture(makeFontTexture()),
      m_font(*mFontTexture, CHAR_WIDTH, CHAR_HEIGHT, NUM_PER_ROW, CHARS, mSDLManager.getRenderer()),
      m_player(makePlayer()), m_screens(*m_player),
      m_initialMessageLines({"Welcome to the game", "? for help (once you've closed this)", "return to start"}) {
    srand(static_cast<unsigned int>(time(NULL)));
    SDL_Renderer *renderer = mSDLManager.getRenderer();

    m_renderTexture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureBlendMode(m_renderTexture, SDL_BLENDMODE_BLEND);

    auto &manager = EntityManager::getInstance();

    auto playerPos = m_player->getPos();

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

    auto statusUI = std::make_unique<StatusUIEntity>(dynamic_cast<PlayerEntity &>(*m_player));
    m_pStatusUI = statusUI.get();
    manager.addEntity(std::move(statusUI));

    EntityBuilder::makeEntityAndAddToInventory<WaterskinEntity>(m_player);
    EntityBuilder::makeEntityAndAddToInventory<GrassTuftEntity>(m_player);
    EntityBuilder::makeEntityAndAddToInventory<GrassTuftEntity>(m_player);
    EntityBuilder::makeEntityAndAddToInventory<TwigEntity>(m_player);
    EntityBuilder::makeEntityAndAddToInventory<TwigEntity>(m_player);
    EntityBuilder::makeEntityAndAddToInventory<TwigEntity>(m_player);

    // Add an example building
    std::vector<std::string> walls = {"++++++++++++", "+          +", "+     +    +", "+ +++++++d++",
                                      "+     +    +", "+     +    +", "+     +    +", "+++d++++++++"};
    auto building = std::make_unique<BuildingWallEntity>(playerPos + Point(10, -10), walls);
    manager.addEntity(std::move(building));

    manager.initialize();
    manager.setTimeOfDay(Time(6, 0));
}

Game::~Game() { SDL_DestroyTexture(m_renderTexture); }

bool Game::processEvent(SDL_Event *e) {
    if (e->type == SDL_EVENT_QUIT)
        return true;
    else if (e->type == SDL_EVENT_KEY_DOWN && e->key.mod & SDL_KMOD_CTRL && e->key.key == SDLK_EQUALS) {
        mSDLManager.rescaleWindow(1.1);
    } else if (e->type == SDL_EVENT_KEY_DOWN && e->key.mod & SDL_KMOD_CTRL && e->key.key == SDLK_MINUS) {
        mSDLManager.rescaleWindow(0.9);
    } else if (m_initialMessage && e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_RETURN)
        m_initialMessage = false;
    else if (!m_initialMessage && e->type == SDL_EVENT_KEY_DOWN) {
        bool screenEnabled = false;
        for (auto &screen : m_screens.getScreens()) {
            if (screen.second->isEnabled()) {
                screen.second->handleInput(e->key);
                screenEnabled = true;
                break;
            }
        }
        if (!screenEnabled) {
            bool quit = false;
            dynamic_cast<PlayerEntity &>(*m_player).handleInput(e->key, quit, m_screens.getScreens());
            if (quit) {
                return true;
            }
        }
    }
    return false;
}

void Game::iterate() {
    beginTime();

    auto renderer = mSDLManager.getRenderer();
    SDL_SetRenderTarget(renderer, m_renderTexture);

    auto &manager = EntityManager::getInstance();
    manager.cleanup();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);

    bool shouldRenderWorld = true;
    Screen *screenToRender = nullptr;
    for (auto &screen : m_screens.getScreens()) {
        if (screen.second->isEnabled()) {
            shouldRenderWorld = screen.second->shouldRenderWorld();
            screenToRender = screen.second.get();
            break;
        }
    }

    if (shouldRenderWorld) {
        m_world.render(m_font, m_player->getWorldPos());
        manager.render(m_font, m_player->getWorldPos(), m_lightMapTexture);
    }

    // Always render status and notification UI
    m_pStatusUI->render(m_font, m_player->getWorldPos());
    NotificationMessageRenderer::getInstance().render(m_font);

    if (screenToRender != nullptr) {
        screenToRender->render(m_font);
    }

    if (m_player->mHp <= 0) {
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(
            std::vector<std::string>{"$[red]You died!", "", "Press return to quit"}, 1);
    }

    if (m_initialMessage)
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(m_initialMessageLines, 1);

    MessageBoxRenderer::getInstance().render(m_font);

    m_font.drawText(std::to_string(m_fps), World::SCREEN_WIDTH - 5, World::SCREEN_HEIGHT - 1);

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderTexture(renderer, m_renderTexture, nullptr, nullptr);

    SDL_RenderPresent(renderer);

    // Cap framerate
    auto frameTimeBeforeCap = endTime();

    auto secondsTooFastBy = 1.0 / MAX_FRAME_RATE - frameTimeBeforeCap;
    if (secondsTooFastBy > 0) {
        SDL_Delay(static_cast<Uint32>(secondsTooFastBy * 1000));
    }

    m_frameTimes.push_back(static_cast<float>(frameTimeBeforeCap + (secondsTooFastBy > 0 ? secondsTooFastBy : 0)));
    m_totalTime += m_frameTimes.back();

    if (m_frameTimes.size() > 60) {
        m_totalTime -= m_frameTimes.front();
        m_frameTimes.pop_front();
    }

    m_fps = m_frameTimes.size() / m_totalTime;
}

std::unique_ptr<Texture> Game::makeFontTexture() {
    auto fontTexture = std::make_unique<Texture>(mSDLManager.getRenderer());
    //    mFontTexture->loadFromFile("resources/curses_800x600.bmp");
    fontTexture->loadFromFile(std::string(SDL_GetBasePath()) + "resources/cursesV2_800x600.bmp");
    return fontTexture;
}

PlayerEntity *Game::makePlayer() {
    auto player = EntityBuilder::makeEntity<PlayerEntity>();
    // Place player in center of world
    // TODO: Fix bug that occurs at (0,0)
    player->setPos(World::SCREEN_WIDTH * 1000 + World::SCREEN_WIDTH / 2,
                   World::SCREEN_HEIGHT * 1000 + World::SCREEN_HEIGHT / 2);
    return player;
}
