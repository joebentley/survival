#include "ChoosingActionEquipmentScreenState.h"

#include "../../../Entity/PlayerEntity.h"
#include "../../Screens/EquipmentScreen.h"
#include "ChoosingNewEquipmentScreenState.h"
#include "ChoosingSlotEquipmentScreenState.h"

void ChoosingActionEquipmentScreenState::onEntry(EquipmentScreen &screen) { screen.setChoosingEquipmentAction(true); }

std::unique_ptr<EquipmentScreenState> ChoosingActionEquipmentScreenState::handleInput(EquipmentScreen &screen,
                                                                                      SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        return std::make_unique<ChoosingSlotEquipmentScreenState>();
    case SDLK_RETURN: {
        auto &player = screen.getPlayer();
        auto chosenSlot = screen.getChosenSlot();
        if (player.hasEquippedInSlot(chosenSlot)) {
            player.unequip(chosenSlot);
        } else if (!player.getInventoryItemsEquippableInSlot(chosenSlot).empty()) {
            return std::make_unique<ChoosingNewEquipmentScreenState>();
        }
        return std::make_unique<ChoosingSlotEquipmentScreenState>();
    }
    }

    return nullptr;
}

void ChoosingActionEquipmentScreenState::onExit(EquipmentScreen &screen) {
    screen.setChoosingEquipmentAction(false);
    screen.setChoosingEquipmentActionIndex(0);
}
