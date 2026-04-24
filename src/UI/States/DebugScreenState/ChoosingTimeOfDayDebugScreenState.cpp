#include "ChoosingTimeOfDayDebugScreenState.h"

#include "../../../Entity/EntityManager.h"
#include "../../Screens/DebugScreen.h"
#include "ChoosingActionDebugScreenState.h"

void ChoosingTimeOfDayDebugScreenState::onEntry(DebugScreen &screen) {
    screen.setChoosingTimeOfDay(true);
    mChosenTime = EntityManager::getInstance().getTimeOfDay();
    screen.setChosenTime(mChosenTime);
}

std::unique_ptr<DebugScreenState> ChoosingTimeOfDayDebugScreenState::handleInput(DebugScreen &screen,
                                                                                 SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_UP:
    case SDLK_K:
        switch (mStringPos) {
        case 0:
            mChosenTime += Time(10, 0);
            break;
        case 1:
            mChosenTime += Time(1, 0);
            break;
        case 2:
            mChosenTime += Time(0, 10);
            break;
        case 3:
            mChosenTime += Time(0, 1);
            break;
        }
        screen.setChosenTime(mChosenTime);
        break;
    case SDLK_DOWN:
    case SDLK_J:
        switch (mStringPos) {
        case 0:
            mChosenTime -= Time(10, 0);
            break;
        case 1:
            mChosenTime -= Time(1, 0);
            break;
        case 2:
            mChosenTime -= Time(0, 10);
            break;
        case 3:
            mChosenTime -= Time(0, 1);
            break;
        }
        screen.setChosenTime(mChosenTime);
        break;
    case SDLK_LEFT:
    case SDLK_H:
        --mStringPos;
        if (mStringPos < 0)
            mStringPos = 3;
        screen.setStringPos(mStringPos);
        break;
    case SDLK_RIGHT:
    case SDLK_L:
        ++mStringPos;
        if (mStringPos > 3)
            mStringPos = 0;
        screen.setStringPos(mStringPos);
        break;
    case SDLK_ESCAPE:
        return std::make_unique<ChoosingActionDebugScreenState>();
    case SDLK_RETURN:
        EntityManager::getInstance().setTimeOfDay(mChosenTime);
        return std::make_unique<ChoosingActionDebugScreenState>();
    }
    return nullptr;
}

void ChoosingTimeOfDayDebugScreenState::onExit(DebugScreen &screen) {
    screen.setChoosingTimeOfDay(false);
    screen.setStringPos(0);
}
