#include "ViewingDescriptionInventoryState.h"

#include "../../Screens/InventoryScreen.h"
#include "ViewingInventoryState.h"

void ViewingDescriptionInventoryState::onEntry(InventoryScreen &screen) { screen.setViewingDescription(true); }

std::unique_ptr<InventoryScreenState> ViewingDescriptionInventoryState::handleInput(InventoryScreen & /*screen*/,
                                                                                    SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        return std::make_unique<ViewingInventoryState>();
    }

    return nullptr;
}

void ViewingDescriptionInventoryState::onExit(InventoryScreen &screen) { screen.setViewingDescription(false); }
