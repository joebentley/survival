#include "ViewingDescriptionInspectionDialogState.h"

#include "../../Screens/InspectionDialog.h"
#include "ChoosingPositionInspectionDialogState.h"

void ViewingDescriptionInspectionDialogState::onEntry(InspectionDialog &screen) { screen.setViewingDescription(true); }

std::unique_ptr<InspectionDialogState>
ViewingDescriptionInspectionDialogState::handleInput(InspectionDialog & /*screen*/, SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        return std::make_unique<ChoosingPositionInspectionDialogState>();
    }

    return nullptr;
}

void ViewingDescriptionInspectionDialogState::onExit(InspectionDialog &screen) { screen.setViewingDescription(false); }
