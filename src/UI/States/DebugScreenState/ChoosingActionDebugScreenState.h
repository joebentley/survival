#pragma once
#include "DebugScreenState.h"

class ChoosingActionDebugScreenState : public DebugScreenState {
  public:
    void onEntry(DebugScreen &screen) override;
    std::unique_ptr<DebugScreenState> handleInput(DebugScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(DebugScreen &screen) override;
};
