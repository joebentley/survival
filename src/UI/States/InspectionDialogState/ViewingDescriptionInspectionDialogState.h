#pragma once

#include "InspectionDialogState.h"

class ViewingDescriptionInspectionDialogState : public InspectionDialogState {
  public:
    void onEntry(InspectionDialog &screen) override;
    std::unique_ptr<InspectionDialogState> handleInput(InspectionDialog &screen, SDL_KeyboardEvent &e) override;
    void onExit(InspectionDialog &screen) override;
};
