#include "ChoosingSlotEquipmentScreenState.h"

#include "../../../Entity/PlayerEntity.h"
#include "../../Screens/EquipmentScreen.h"
#include "ChoosingActionEquipmentScreenState.h"
#include "ChoosingNewEquipmentScreenState.h"

void ChoosingSlotEquipmentScreenState::onEntry(EquipmentScreen &screen) { mChosenSlot = screen.getChosenSlot(); }

std::unique_ptr<EquipmentScreenState> ChoosingSlotEquipmentScreenState::handleInput(EquipmentScreen &screen,
                                                                                    SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        screen.reset();
        screen.disable();
        break;
    case SDLK_J:
        ++mChosenSlot;
        screen.setChosenSlot(mChosenSlot);
        break;
    case SDLK_K:
        --mChosenSlot;
        screen.setChosenSlot(mChosenSlot);
        break;
    case SDLK_RETURN:
        auto &player = screen.getPlayer();
        if (!player.hasEquippedInSlot(mChosenSlot) && !player.getInventoryItemsEquippableInSlot(mChosenSlot).empty())
            return std::make_unique<ChoosingNewEquipmentScreenState>();
        else
            return std::make_unique<ChoosingActionEquipmentScreenState>();
    }

    return nullptr;
}

void ChoosingSlotEquipmentScreenState::onExit(EquipmentScreen & /*screen*/) {}
