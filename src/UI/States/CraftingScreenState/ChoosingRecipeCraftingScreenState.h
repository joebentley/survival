#pragma once
#include "CraftingScreenState.h"

class ChoosingRecipeCraftingScreenState : public CraftingScreenState {
  public:
    void onEntry(CraftingScreen &screen) override;
    std::unique_ptr<CraftingScreenState> handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(CraftingScreen & /*screen*/) override {};

  private:
    int mChosenRecipe{0};
};
