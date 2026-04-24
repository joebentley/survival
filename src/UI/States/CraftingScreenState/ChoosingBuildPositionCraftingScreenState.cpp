#include "ChoosingBuildPositionCraftingScreenState.h"

#include "../../../Entity/EntityManager.h"
#include "../../../Entity/PlayerEntity.h"
#include "../../Screens/CraftingScreen.h"
#include "ChoosingRecipeCraftingScreenState.h"

void ChoosingBuildPositionCraftingScreenState::tryToBuildAtPosition(CraftingScreen &screen, Point posOffset) {
    auto p = posOffset + screen.getPlayer().getPos();
    if (EntityManager::getInstance().getEntitiesAtPosFaster(p).empty()) {
        mHaveChosenPositionInWorld = true;
        screen.buildItem(p);
    } else {
        screen.setCouldNotBuildAtPosition(true);
    }
}

void ChoosingBuildPositionCraftingScreenState::onEntry(CraftingScreen &screen) {
    screen.setChoosingPositionInWorld(true);
}

std::unique_ptr<CraftingScreenState> ChoosingBuildPositionCraftingScreenState::handleInput(CraftingScreen &screen,
                                                                                           SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_J:
        tryToBuildAtPosition(screen, Point{0, 1});
        break;
    case SDLK_K:
        tryToBuildAtPosition(screen, Point{0, -1});
        break;
    case SDLK_L:
        tryToBuildAtPosition(screen, Point{1, 0});
        break;
    case SDLK_H:
        tryToBuildAtPosition(screen, Point{-1, 0});
        break;
    case SDLK_Y:
        tryToBuildAtPosition(screen, Point{-1, -1});
        break;
    case SDLK_U:
        tryToBuildAtPosition(screen, Point{1, -1});
        break;
    case SDLK_B:
        tryToBuildAtPosition(screen, Point{-1, 1});
        break;
    case SDLK_N:
        tryToBuildAtPosition(screen, Point{1, 1});
        break;
    }

    if (mHaveChosenPositionInWorld)
        return std::make_unique<ChoosingRecipeCraftingScreenState>();

    return nullptr;
}

void ChoosingBuildPositionCraftingScreenState::onExit(CraftingScreen &screen) {
    screen.setChoosingPositionInWorld(true);
}
