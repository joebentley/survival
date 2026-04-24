#pragma once

#include "../../UIState.h"
#include "Screen.h"

struct DebugScreen : Screen {
    DebugScreen();

    const std::vector<std::string> cDebugOptions = {"1) Change time of day"};

    void enable() override {
        mChoosingDebugAction = false;
        Screen::enable();
    }

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;

    void setChoosingDebugAction(bool choosingDebugAction);
    void setChoosingTimeOfDay(bool choosingTimeOfDay);
    void setChosenTime(const Time &chosenTime);

    void setStringPos(int stringPos);

  private:
    std::unique_ptr<DebugScreenState> mState;
    bool mChoosingDebugAction{false};
    bool mChoosingTimeOfDay{false};
    int mStringPos{0};

    Time mChosenTime;
};
