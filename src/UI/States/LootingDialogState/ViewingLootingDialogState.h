#pragma once

#include "LootingDialogState.h"

class ViewingLootingDialogState : public LootingDialogState {
  public:
    void onEntry(LootingDialog &screen) override;
    std::unique_ptr<LootingDialogState> handleInput(LootingDialog &screen, SDL_KeyboardEvent &e) override;
    void onExit(LootingDialog &screen) override;

  private:
    int mChosenIndex{0};
};
