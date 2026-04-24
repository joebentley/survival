#pragma once

#include "InventoryScreenState.h"

class ViewingInventoryState : public InventoryScreenState {
  public:
    void onEntry(InventoryScreen &screen) override;
    std::unique_ptr<InventoryScreenState> handleInput(InventoryScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(InventoryScreen &screen) override;

  private:
    int mChosenIndex{0};
};
