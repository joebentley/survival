#pragma once

#include "../../Entity/EquipmentSlot.h"
#include "../../UIState.h"
#include "Screen.h"

struct PlayerEntity;
struct EquipmentScreen : Screen {
    explicit EquipmentScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;

    /// Render equipment screen
    /// \param font Font object to render using
    void render(Font &font) override;

    void reset();
    void enable() override;

    PlayerEntity &getPlayer();

    EquipmentSlot getChosenSlot() const;
    void setChosenSlot(EquipmentSlot chosenSlot);

    void setChoosingEquipmentAction(bool choosingEquipmentAction);
    void setChoosingEquipmentActionIndex(int choosingEquipmentActionIndex);
    void setChoosingNewEquipment(bool choosingNewEquipment);
    void setChoosingNewEquipmentIndex(int choosingNewEquipmentIndex);

  private:
    PlayerEntity &mPlayer;
    EquipmentSlot mChosenSlot{EquipmentSlot::HEAD};
    bool mChoosingEquipmentAction{false};
    int mChoosingEquipmentActionIndex{0};
    bool mChoosingNewEquipment{false};
    int mChoosingNewEquipmentIndex{0};

    std::unique_ptr<EquipmentScreenState> mState{std::make_unique<ChoosingSlotEquipmentScreenState>()};
};
