#pragma once

#include "CraftingScreen.h"
#include "DebugScreen.h"
#include "EquipmentScreen.h"
#include "HelpScreen.h"
#include "InspectionDialog.h"
#include "InventoryScreen.h"
#include "LootingDialog.h"
#include "NotificationMessageScreen.h"
#include "Screen.h"

struct PlayerEntity;
/// Initializes and sets up all screens in the game
struct Screens {
    explicit Screens(PlayerEntity &player) {
        mScreens[ScreenType::NOTIFICATION] = std::make_unique<NotificationMessageScreen>();
        mScreens[ScreenType::INVENTORY] = std::make_unique<InventoryScreen>(player);
        mScreens[ScreenType::LOOTING] = std::make_unique<LootingDialog>(player);
        mScreens[ScreenType::INSPECTION] = std::make_unique<InspectionDialog>(player);
        mScreens[ScreenType::CRAFTING] = std::make_unique<CraftingScreen>(player);
        mScreens[ScreenType::EQUIPMENT] = std::make_unique<EquipmentScreen>(player);
        mScreens[ScreenType::HELP] = std::make_unique<HelpScreen>();
        mScreens[ScreenType::DEBUG] = std::make_unique<DebugScreen>();
    }

    /// Return a direct reference to the underlying screens
    std::unordered_map<ScreenType, std::unique_ptr<Screen>> &getScreens() { return mScreens; }

  private:
    std::unordered_map<ScreenType, std::unique_ptr<Screen>> mScreens;
};
