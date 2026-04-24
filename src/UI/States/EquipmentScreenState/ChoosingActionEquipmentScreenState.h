#pragma once

#include "EquipmentScreenState.h"

class ChoosingActionEquipmentScreenState : public EquipmentScreenState {
  public:
    void onEntry(EquipmentScreen &screen) override;
    std::unique_ptr<EquipmentScreenState> handleInput(EquipmentScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(EquipmentScreen &screen) override;
};
