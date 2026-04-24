#pragma once
#include "ApplyableBehaviour.h"

// TODO: enemies should be able to use it to heal themselves
/// Represents an item that heals the player, not exceeding their maximum health
struct HealingItemBehaviour : ApplyableBehaviour {
    /// Initialize the behaviour
    /// \param parent parent entity
    /// \param healingAmount amount to heal the player
    HealingItemBehaviour(Entity &parent, float healingAmount);

    float healingAmount;

    void apply() override;
};
