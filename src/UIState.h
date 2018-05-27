#ifndef SURVIVAL_STATE_H
#define SURVIVAL_STATE_H

#include <SDL2/SDL.h>
#include <memory>
#include "Point.h"


struct InventoryScreen;
struct LootingDialog;
struct CraftingScreen;
struct EquipmentScreen;

template<typename T>
class UIState {
public:
    virtual void onEntry(T &screen) = 0;
    virtual std::unique_ptr<UIState<T>> handleInput(T &screen, SDL_KeyboardEvent &e) = 0;
    virtual void onExit(T &screen) = 0;
};

typedef UIState<InventoryScreen> InventoryScreenState;
typedef UIState<LootingDialog> LootingDialogState;
typedef UIState<CraftingScreen> CraftingScreenState;
typedef UIState<EquipmentScreen> EquipmentScreenState;

class ViewingInventoryState : public InventoryScreenState {
public:
    void onEntry(InventoryScreen &screen) override;
    std::unique_ptr<InventoryScreenState> handleInput(InventoryScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(InventoryScreen &screen) override;

private:
    int mChosenIndex {0};
};

class ViewingDescriptionInventoryState : public InventoryScreenState {
public:
    void onEntry(InventoryScreen &screen) override;
    std::unique_ptr<InventoryScreenState> handleInput(InventoryScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(InventoryScreen &screen) override;
};

class ViewingLootingDialogState : public LootingDialogState {
public:
    void onEntry(LootingDialog &screen) override;
    std::unique_ptr<LootingDialogState> handleInput(LootingDialog &screen, SDL_KeyboardEvent &e) override;
    void onExit(LootingDialog &screen) override;

private:
    int mChosenIndex {0};
};

class ViewingDescriptionLootingDialogState : public LootingDialogState {
public:
    void onEntry(LootingDialog &screen) override;
    std::unique_ptr<LootingDialogState> handleInput(LootingDialog &screen, SDL_KeyboardEvent &e) override;
    void onExit(LootingDialog &screen) override;
};

class ShowingTooMuchWeightMessageLootingDialogState : public LootingDialogState {
public:
    void onEntry(LootingDialog &screen) override;
    std::unique_ptr<LootingDialogState> handleInput(LootingDialog &screen, SDL_KeyboardEvent &e) override;
    void onExit(LootingDialog &screen) override;
};

class ChoosingRecipeCraftingScreenState : public CraftingScreenState {
public:
    void onEntry(CraftingScreen &screen) override;
    std::unique_ptr<CraftingScreenState> handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(CraftingScreen &screen) override {};

private:
    int mChosenRecipe {0};
};

class ChoosingIngredientCraftingScreenState : public CraftingScreenState {
public:
    void onEntry(CraftingScreen &screen) override;
    std::unique_ptr<CraftingScreenState> handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(CraftingScreen &screen) override {};

private:
    int mChosenIngredient {0};
};

class ChoosingMaterialCraftingScreenState : public CraftingScreenState {
public:
    void onEntry(CraftingScreen &screen) override;
    std::unique_ptr<CraftingScreenState> handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(CraftingScreen &screen) override {};

private:
    int mChosenMaterial {0};
};

class ChoosingBuildPositionCraftingScreenState : public CraftingScreenState {
public:
    void onEntry(CraftingScreen &screen) override;
    std::unique_ptr<CraftingScreenState> handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) override;
    void onExit(CraftingScreen &screen) override;

private:
    void tryToBuildAtPosition(CraftingScreen &screen, Point posOffset);

    bool mHaveChosenPositionInWorld {false};
};

#endif //SURVIVAL_STATE_H
