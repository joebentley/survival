#include "ChoosingActionDebugScreenState.h"

#include "../../Screens/DebugScreen.h"
#include "ChoosingTimeOfDayDebugScreenState.h"

void ChoosingActionDebugScreenState::onEntry(DebugScreen &screen) { screen.setChoosingDebugAction(true); }

std::unique_ptr<DebugScreenState> ChoosingActionDebugScreenState::handleInput(DebugScreen &screen,
                                                                              SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        screen.disable();
        return nullptr;
    case SDLK_1:
        return std::make_unique<ChoosingTimeOfDayDebugScreenState>();
    }

    return nullptr;
}

void ChoosingActionDebugScreenState::onExit(DebugScreen &screen) { screen.setChoosingDebugAction(false); }
