#ifndef SURVIVAL_STATE_H
#define SURVIVAL_STATE_H

#include <SDL2/SDL.h>
#include <memory>

struct InventoryScreen;

class InventoryScreenState {
public:
    virtual void onEntry(InventoryScreen &screen) = 0;
    virtual std::unique_ptr<InventoryScreenState> handleInput(InventoryScreen &screen, SDL_KeyboardEvent &e) = 0;
    virtual void onExit(InventoryScreen &screen) = 0;
};

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


struct LootingDialog;

class LootingDialogState {
public:
    virtual void onEntry(LootingDialog &screen) = 0;
    virtual std::unique_ptr<LootingDialogState> handleInput(LootingDialog &screen, SDL_KeyboardEvent &e) = 0;
    virtual void onExit(LootingDialog &screen) = 0;
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


#endif //SURVIVAL_STATE_H
