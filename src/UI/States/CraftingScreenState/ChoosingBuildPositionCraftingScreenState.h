#pragma once
#include "CraftingScreenState.h"

struct Point;
class ChoosingBuildPositionCraftingScreenState : public CraftingScreenState {
  public:
    void onEntry(CraftingScreen &screen) override;
    std::unique_ptr<CraftingScreenState> handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(CraftingScreen &screen) override;

  private:
    void tryToBuildAtPosition(CraftingScreen &screen, Point posOffset);

    bool mHaveChosenPositionInWorld{false};
};
