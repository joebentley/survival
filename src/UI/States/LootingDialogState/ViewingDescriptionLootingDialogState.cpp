#include "ViewingDescriptionLootingDialogState.h"

#include "../../Screens/LootingDialog.h"
#include "ViewingLootingDialogState.h"

void ViewingDescriptionLootingDialogState::onEntry(LootingDialog &screen) { screen.setViewingDescription(true); }

std::unique_ptr<LootingDialogState> ViewingDescriptionLootingDialogState::handleInput(LootingDialog & /*screen*/,
                                                                                      SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        return std::make_unique<ViewingLootingDialogState>();
    }

    return nullptr;
}

void ViewingDescriptionLootingDialogState::onExit(LootingDialog &screen) { screen.setViewingDescription(false); }
