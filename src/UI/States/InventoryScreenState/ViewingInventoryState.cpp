#include "ViewingInventoryState.h"
#include "../../../Behaviour/Item/ApplyableBehaviour.h"
#include "../../../Entity/PlayerEntity.h"
#include "../../Screens/InventoryScreen.h"
#include "ViewingDescriptionInventoryState.h"

void ViewingInventoryState::onEntry(InventoryScreen & /*screen*/) {}

std::unique_ptr<InventoryScreenState> ViewingInventoryState::handleInput(InventoryScreen &screen,
                                                                         SDL_KeyboardEvent &e) {
    auto &player = screen.getPlayer();

    mChosenIndex = screen.getChosenIndex();

    switch (e.key) {
    case SDLK_ESCAPE:
        screen.disable();
        break;
    case SDLK_J:
        if (!player.isInventoryEmpty()) {
            if ((size_t)mChosenIndex < (player.getInventorySize() - 1))
                mChosenIndex++;
            else
                mChosenIndex = 0;
        }
        screen.setChosenIndex(mChosenIndex);
        break;
    case SDLK_K:
        if (!player.isInventoryEmpty()) {
            if (mChosenIndex > 0)
                mChosenIndex--;
            else
                mChosenIndex = (int)player.getInventorySize() - 1;
        }
        screen.setChosenIndex(mChosenIndex);
        break;
    case SDLK_D:
        if (!player.isInventoryEmpty()) {
            auto itemID = player.getInventoryItemID(mChosenIndex);
            if (player.hasEquipped(itemID))
                player.unequip(itemID);

            player.dropItem(mChosenIndex);

            if (player.getInventorySize() - 1 < (size_t)mChosenIndex) {
                mChosenIndex--;
                screen.setChosenIndex(mChosenIndex);
            }
        }
        break;
    case SDLK_E:
        if (!player.isInventoryEmpty()) {
            auto item = player.getInventoryItem(mChosenIndex);
            auto eatable = item->getProperty("Eatable");
            if (eatable != nullptr) {
                player.addHunger(std::any_cast<float>(eatable->getValue()));
                player.removeFromInventory(mChosenIndex);
                item->destroy();
                if (player.getInventorySize() - 1 < (size_t)mChosenIndex) {
                    mChosenIndex--;
                    screen.setChosenIndex(mChosenIndex);
                }
            }
        }
        break;
    case SDLK_A:
        if (!player.isInventoryEmpty()) {
            auto item = player.getInventoryItem(mChosenIndex);
            if (item->hasBehaviour("ApplyableBehaviour")) {
                dynamic_cast<ApplyableBehaviour &>(*(*item).getBehaviourByID("ApplyableBehaviour")).apply();
                if (player.getInventorySize() - 1 < (size_t)mChosenIndex) {
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

void ViewingInventoryState::onExit(InventoryScreen & /*screen*/) {}
