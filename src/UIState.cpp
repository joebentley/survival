//
// Created by Joe Bentley on 25/05/2018.
//

#include "UIState.h"
#include "UI.h"

void ViewingInventoryState::onEntry(InventoryScreen &screen) {
    mChosenIndex = screen.getChosenIndex();
}

std::unique_ptr<InventoryScreenState>
ViewingInventoryState::handleInput(InventoryScreen &screen, SDL_KeyboardEvent &e) {
    auto &player = screen.getPlayer();

    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.disable();
            break;
        case SDLK_j:
            if (!player.isInventoryEmpty()) {
                if (mChosenIndex < (player.getInventorySize() - 1))
                    mChosenIndex++;
                else
                    mChosenIndex = 0;
            }
            screen.setChosenIndex(mChosenIndex);
            break;
        case SDLK_k:
            if (!player.isInventoryEmpty()) {
                if (mChosenIndex > 0)
                    mChosenIndex--;
                else
                    mChosenIndex = (int) player.getInventorySize() - 1;
            }
            screen.setChosenIndex(mChosenIndex);
            break;
        case SDLK_d:
            if (!player.isInventoryEmpty()) {
                auto itemID = player.getInventoryItemID(mChosenIndex);
                if (player.hasEquipped(itemID))
                    player.unequip(itemID);

                player.dropItem(mChosenIndex);

                if (player.getInventorySize() - 1 < mChosenIndex) {
                    mChosenIndex--;
                    screen.setChosenIndex(mChosenIndex);
                }
            }
            break;
        case SDLK_e:
            if (!player.isInventoryEmpty()) {
                auto item = player.getInventoryItem(mChosenIndex);
                if (item->hasBehaviour("EatableBehaviour")) {
                    player.addHunger(dynamic_cast<EatableBehaviour &>(*(*item).getBehaviourByID("EatableBehaviour")).hungerRestoration);
                    player.removeFromInventory(mChosenIndex);
                    item->destroy();
                    if (player.getInventorySize() - 1 < mChosenIndex) {
                        mChosenIndex--;
                        screen.setChosenIndex(mChosenIndex);
                    }
                }
            }
            break;
        case SDLK_a:
            if (!player.isInventoryEmpty()) {
                auto item = player.getInventoryItem(mChosenIndex);
                if (item->hasBehaviour("ApplyableBehaviour")) {
                    dynamic_cast<ApplyableBehaviour&>(*(*item).getBehaviourByID("ApplyableBehaviour")).apply();
                    if (player.getInventorySize() - 1 < mChosenIndex) {
                        mChosenIndex--;
                        screen.setChosenIndex(mChosenIndex);
                    }
                }
            }
            break;
        case SDLK_RETURN:
            if (!player.isInventoryEmpty())
                return std::make_unique<ViewingDescriptionInventoryState>();
            break;
    }

    return nullptr;
}

void ViewingInventoryState::onExit(InventoryScreen &screen) {
}

void ViewingDescriptionInventoryState::onEntry(InventoryScreen &screen) {
    screen.setViewingDescription(true);
}

std::unique_ptr<InventoryScreenState> ViewingDescriptionInventoryState::handleInput(InventoryScreen &screen,
                                                                                    SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            return std::make_unique<ViewingInventoryState>();
    }

    return nullptr;
}

void ViewingDescriptionInventoryState::onExit(InventoryScreen &screen) {
    screen.setViewingDescription(false);
}

void ViewingLootingDialogState::onEntry(LootingDialog &screen) {
    mChosenIndex = screen.getChosenIndex();
}

std::unique_ptr<LootingDialogState>
ViewingLootingDialogState::handleInput(LootingDialog &screen, SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.disable();
            break;
        case SDLK_RETURN:
            return std::make_unique<ViewingDescriptionLootingDialogState>();
        case SDLK_j:
            if (!screen.getItemsToShow().empty()) {
                if (mChosenIndex < (screen.getItemsToShow().size() - 1))
                    mChosenIndex++;
                else
                    mChosenIndex = 0;
                screen.setChosenIndex(mChosenIndex);
            }
            break;
        case SDLK_k:
            if (!screen.getItemsToShow().empty()) {
                if (mChosenIndex > 0)
                    mChosenIndex--;
                else
                    mChosenIndex = (int) screen.getItemsToShow().size() - 1;
                screen.setChosenIndex(mChosenIndex);
            }
            break;
        case SDLK_g:
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

void ViewingLootingDialogState::onExit(LootingDialog &screen) {
}

void ViewingDescriptionLootingDialogState::onEntry(LootingDialog &screen) {
    screen.setViewingDescription(true);
}

std::unique_ptr<LootingDialogState>
ViewingDescriptionLootingDialogState::handleInput(LootingDialog &screen, SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            return std::make_unique<ViewingLootingDialogState>();
    }

    return nullptr;
}

void ViewingDescriptionLootingDialogState::onExit(LootingDialog &screen) {
    screen.setViewingDescription(false);
}

void ShowingTooMuchWeightMessageLootingDialogState::onEntry(LootingDialog &screen) {
    screen.setShowingTooMuchWeightMessage(true);
}

std::unique_ptr<LootingDialogState>
ShowingTooMuchWeightMessageLootingDialogState::handleInput(LootingDialog &screen, SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_RETURN:
        case SDLK_ESCAPE:
            return std::make_unique<ViewingLootingDialogState>();
    }

    return nullptr;
}

void ShowingTooMuchWeightMessageLootingDialogState::onExit(LootingDialog &screen) {
    screen.setShowingTooMuchWeightMessage(false);
}
