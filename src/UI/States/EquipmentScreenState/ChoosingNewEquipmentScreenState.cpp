#include "ChoosingNewEquipmentScreenState.h"

#include "../../../Entity/PlayerEntity.h"
#include "../../Screens/EquipmentScreen.h"
#include "ChoosingSlotEquipmentScreenState.h"

void ChoosingNewEquipmentScreenState::onEntry(EquipmentScreen &screen) { screen.setChoosingNewEquipment(true); }

std::unique_ptr<EquipmentScreenState> ChoosingNewEquipmentScreenState::handleInput(EquipmentScreen &screen,
                                                                                   SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        return std::make_unique<ChoosingSlotEquipmentScreenState>();
    case SDLK_J: {
        auto equippableIDs = screen.getPlayer().getInventoryItemsEquippableInSlot(screen.getChosenSlot());
        if ((size_t)mChoosingNewEquipmentIndex == equippableIDs.size() - 1)
            mChoosingNewEquipmentIndex = 0;
        else
            ++mChoosingNewEquipmentIndex;
        screen.setChoosingNewEquipmentIndex(mChoosingNewEquipmentIndex);
        break;
    }
    case SDLK_K: {
        auto equippableIDs = screen.getPlayer().getInventoryItemsEquippableInSlot(screen.getChosenSlot());
        if (mChoosingNewEquipmentIndex == 0)
            mChoosingNewEquipmentIndex = static_cast<int>(equippableIDs.size()) - 1;
        else
            --mChoosingNewEquipmentIndex;
        screen.setChoosingNewEquipmentIndex(mChoosingNewEquipmentIndex);
        break;
    }
    case SDLK_RETURN: {
        auto &player = screen.getPlayer();
        auto chosenSlot = screen.getChosenSlot();
        auto equippableIDs = player.getInventoryItemsEquippableInSlot(chosenSlot);
        player.equip(chosenSlot, equippableIDs[mChoosingNewEquipmentIndex]);
        screen.reset();
        return std::make_unique<ChoosingSlotEquipmentScreenState>();
    }
    }

    return nullptr;
}

void ChoosingNewEquipmentScreenState::onExit(EquipmentScreen &screen) { screen.setChoosingNewEquipment(false); }
