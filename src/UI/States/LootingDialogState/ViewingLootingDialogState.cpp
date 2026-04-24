#include "ViewingLootingDialogState.h"

#include "../../../Entity/PlayerEntity.h"
#include "../../Screens/LootingDialog.h"
#include "ShowingTooMuchWeightMessageLootingDialogState.h"
#include "ViewingDescriptionLootingDialogState.h"

void ViewingLootingDialogState::onEntry(LootingDialog &screen) { mChosenIndex = screen.getChosenIndex(); }

std::unique_ptr<LootingDialogState> ViewingLootingDialogState::handleInput(LootingDialog &screen,
                                                                           SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        screen.disable();
        break;
    case SDLK_RETURN:
        return std::make_unique<ViewingDescriptionLootingDialogState>();
    case SDLK_J:
        if (!screen.getItemsToShow().empty()) {
            if ((size_t)mChosenIndex < (screen.getItemsToShow().size() - 1))
                mChosenIndex++;
            else
                mChosenIndex = 0;
            screen.setChosenIndex(mChosenIndex);
        }
        break;
    case SDLK_K:
        if (!screen.getItemsToShow().empty()) {
            if (mChosenIndex > 0)
                mChosenIndex--;
            else
                mChosenIndex = (int)screen.getItemsToShow().size() - 1;
            screen.setChosenIndex(mChosenIndex);
        }
        break;
    case SDLK_G:
        auto &itemsToShow = screen.getItemsToShow();
        auto entityToTransferFrom = screen.getEntityToTransferFrom();
        if (screen.getPlayer().addToInventory(itemsToShow[mChosenIndex]->mID)) {
            if (entityToTransferFrom != nullptr) {
                entityToTransferFrom->dropItem(mChosenIndex);
            }

            itemsToShow.erase(itemsToShow.begin() + mChosenIndex);
            mChosenIndex = 0;
            screen.setChosenIndex(mChosenIndex);
        } else {
            return std::make_unique<ShowingTooMuchWeightMessageLootingDialogState>();
        }
        break;
    }

    return nullptr;
}

void ViewingLootingDialogState::onExit(LootingDialog & /*screen*/) {}