#pragma once
#include "Entity.h"
#include <SDL3/SDL_events.h>

struct Screen;
enum class ScreenType;
/// A special entity with ID "Player"
struct PlayerEntity : Entity {
    /// 1 is full, <0.3 is starving
    float hunger;
    /// how much hunger should decrease per tick
    float hungerRate;
    /// hp loss per tick while starving
    float hungerDamageRate;
    /// whether or not player is attacking something
    bool attacking{false};
    /// whether or not showing "too much weight message" on screen
    bool showingTooMuchWeightMessage{false};

    explicit PlayerEntity()
        : Entity("Player", "You, the player", "$[white]$(dwarf)", 10.0f, 10.0f, 0.1f, 1, 4, 100), hunger(1),
          hungerRate(0.005f), hungerDamageRate(0.15f) {
        mRenderingLayer = -1;
    }

    /// Try to attack the entity at attackPos
    bool attack(const Point &attackPos);
    /// Tick entity, taking hunger into account
    void tick() override;
    /// Handle most of the ingame interaction
    void handleInput(SDL_KeyboardEvent &e, bool &quit,
                     std::unordered_map<ScreenType, std::unique_ptr<Screen>> &screens);
    /// Same as usual render except can show a "too much weight" message box, and offload additional rendering to
    /// "mEntityInteractingWith"
    void render(Font &font, Point currentWorldPos) override;
    /// Overrides usual entity addToInventory but displays a nice notification
    bool addToInventory(const std::string &ID) override;

    /// Add hunger, not exceeding 1.0f
    void addHunger(float hunger);

  private:
    bool interactingWithEntity{false};
    std::string mEntityInteractingWith;
};
