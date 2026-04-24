#pragma once
#include "../../../Point.h"
#include "InspectionDialogState.h"

class ChoosingPositionInspectionDialogState : public InspectionDialogState {
  public:
    void onEntry(InspectionDialog &screen) override;
    std::unique_ptr<InspectionDialogState> handleInput(InspectionDialog &screen, SDL_KeyboardEvent &e) override;
    void onExit(InspectionDialog &screen) override;

  private:
    inline Point clipToScreenEdge(InspectionDialog &screen, const Point &p) const;

    Point mChosenPoint;
    int mChosenIndex{0};
};
