#pragma once

#include "../../UIState.h"
#include "Screen.h"

struct PlayerEntity;
struct InspectionDialog : Screen {
    explicit InspectionDialog(PlayerEntity &player) : Screen(true), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;

    void enableAtPoint(Point initialPoint) {
        mChosenPoint = initialPoint;
        Screen::enable();
    }

    PlayerEntity &getPlayer();
    const Point &getChosenPoint() const;
    void setChosenPoint(const Point &chosenPoint);
    bool isThereAnEntity() const;
    void setViewingDescription(bool viewingDescription);

    bool isSelectingFromMultipleOptions() const;

    void setChosenIndex(int chosenIndex);

  private:
    PlayerEntity &mPlayer;
    Point mChosenPoint;
    bool mSelectingFromMultipleOptions{false};
    int mChosenIndex{0};
    bool mViewingDescription{false};
    bool mThereIsAnEntity{false};

    std::unique_ptr<InspectionDialogState> mState{std::make_unique<ChoosingPositionInspectionDialogState>()};
};