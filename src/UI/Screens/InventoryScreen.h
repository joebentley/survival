#pragma once

#include "../../UI/States/InventoryScreenState/InventoryScreenState.h"
#include "../../UI/States/InventoryScreenState/ViewingInventoryState.h"
#include "Screen.h"

struct PlayerEntity;
struct InventoryScreen : Screen {
    static const int X_OFFSET = 4;
    static const int X_STATUS_OFFSET = 10;
    static const int Y_OFFSET = 4;
    static const int WORD_WRAP_COLUMN = 40;

    explicit InventoryScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;

    void enable() override {
        mChosenIndex = 0;
        Screen::enable();
    }

    PlayerEntity &getPlayer() const;

    int getChosenIndex() const;
    void setChosenIndex(int chosenIndex);

    void setViewingDescription(bool viewingDescription);

  private:
    PlayerEntity &mPlayer;
    int mChosenIndex{0};
    bool mViewingDescription{false};
    std::unique_ptr<InventoryScreenState> mState{std::make_unique<ViewingInventoryState>()};
};
