#include "DebugScreen.h"

#include "../MessageBoxRenderer.h"

DebugScreen::DebugScreen() : Screen(false) {
    mState = std::make_unique<ChoosingActionDebugScreenState>();
    mState->onEntry(*this);
}

void DebugScreen::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }
}

void DebugScreen::render(Font &font) {
    const int yPadding = 2;
    const int xPadding = 2;
    int y = 0;

    for (const auto &line : cDebugOptions) {
        font.drawText(line, xPadding, yPadding + y++);
    }

    if (mChoosingTimeOfDay) {
        std::string timeString = mChosenTime.toString();
        std::string bColorString = "${grey}";
        timeString.insert(static_cast<unsigned long>(mStringPos + (mStringPos >= 2 ? 1 : 0)), bColorString);
        timeString.insert(mStringPos + bColorString.size() + 1 + (mStringPos >= 2 ? 1 : 0), "${transparent}");
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(timeString, 1);
    }
}

void DebugScreen::setChoosingDebugAction(bool choosingDebugAction) { mChoosingDebugAction = choosingDebugAction; }

void DebugScreen::setChoosingTimeOfDay(bool choosingTimeOfDay) { mChoosingTimeOfDay = choosingTimeOfDay; }

void DebugScreen::setChosenTime(const Time &chosenTime) { mChosenTime = chosenTime; }

void DebugScreen::setStringPos(int stringPos) { mStringPos = stringPos; }
