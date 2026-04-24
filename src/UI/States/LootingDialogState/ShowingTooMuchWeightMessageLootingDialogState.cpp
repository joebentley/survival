#include "ShowingTooMuchWeightMessageLootingDialogState.h"

#include "../../Screens/LootingDialog.h"
#include "ViewingLootingDialogState.h"

void ShowingTooMuchWeightMessageLootingDialogState::onEntry(LootingDialog &screen) {
    screen.setShowingTooMuchWeightMessage(true);
}

std::unique_ptr<LootingDialogState>
ShowingTooMuchWeightMessageLootingDialogState::handleInput(LootingDialog & /*screen*/, SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_RETURN:
    case SDLK_ESCAPE:
        return std::make_unique<ViewingLootingDialogState>();
    }

    return nullptr;
}

void ShowingTooMuchWeightMessageLootingDialogState::onExit(LootingDialog &screen) {
    screen.setShowingTooMuchWeightMessage(false);
}
