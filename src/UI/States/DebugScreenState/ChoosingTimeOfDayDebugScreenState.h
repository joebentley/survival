#pragma once

#include "../../../Time.h"
#include "DebugScreenState.h"

class ChoosingTimeOfDayDebugScreenState : public DebugScreenState {
  public:
    void onEntry(DebugScreen &screen) override;
    std::unique_ptr<DebugScreenState> handleInput(DebugScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(DebugScreen &screen) override;

  private:
    int mStringPos{0};

    Time mChosenTime;
};
