#pragma once

#include "../../../Entity/EquipmentSlot.h"
#include "EquipmentScreenState.h"

class ChoosingSlotEquipmentScreenState : public EquipmentScreenState {
  public:
    void onEntry(EquipmentScreen &screen) override;
    std::unique_ptr<EquipmentScreenState> handleInput(EquipmentScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(EquipmentScreen &screen) override;

  private:
    EquipmentSlot mChosenSlot{EquipmentSlot::HEAD};
};
