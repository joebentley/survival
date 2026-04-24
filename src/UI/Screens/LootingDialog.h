#pragma once

#include "../../UIState.h"
#include "Screen.h"

struct PlayerEntity;
struct LootingDialog : Screen {
    explicit LootingDialog(PlayerEntity &player) : Screen(true), mPlayer(player) {}

    void showItemsToLoot(std::vector<Entity *> items);
    void showItemsToLoot(std::vector<Entity *> items, Entity *entityToTransferFrom);

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;

    void setViewingDescription(bool viewingDescription);

    void setShowingTooMuchWeightMessage(bool showingTooMuchWeightMessage);

    int getChosenIndex() const;
    void setChosenIndex(int chosenIndex);

    PlayerEntity &getPlayer() const;
    std::vector<Entity *> &getItemsToShow();
    Entity *getEntityToTransferFrom() const;

  private:
    const int DIALOG_WIDTH = 30;

    bool mViewingDescription{false};
    bool mShowingTooMuchWeightMessage{false};
    PlayerEntity &mPlayer;
    std::vector<Entity *> mItemsToShow;
    int mChosenIndex{0};
    Entity *mEntityToTransferFrom{nullptr};

    std::unique_ptr<LootingDialogState> mState{std::make_unique<ViewingLootingDialogState>()};
};
