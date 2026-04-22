#pragma once

#include "../../Entity/Entity.h"
#include "../../Entity/EntityManager.h"
#include "../Behaviour.h"
#include "ApplyableBehaviour.h"

// TODO: enemies should be able to use it to heal themselves
/// Represents an item that heals the player, not exceeding their maximum health
struct HealingItemBehaviour : ApplyableBehaviour {
    /// Initialize the behaviour
    /// \param parent parent entity
    /// \param healingAmount amount to heal the player
    HealingItemBehaviour(Entity &parent, float healingAmount)
        : ApplyableBehaviour("HealingItemBehaviour", parent), healingAmount(healingAmount) {}

    float healingAmount;

    void apply() override {
        auto player = EntityManager::getInstance().getEntityByID("Player");
        player->addHealth(healingAmount);
        // destroy the parent entity once it is used
        player->removeFromInventory(mParent.mID);
        mParent.destroy();
    }
};
