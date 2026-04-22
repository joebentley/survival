#pragma once

#include "../../Entity/Entity.h"
#include "../../Entity/EntityManager.h"
#include "../Behaviour.h"

/// Keep the parent entity's inventory stocked with the entity given by T
/// WARNING: you should add the initial item in your Entities' constructor, or otherwise you could get a huge number
/// of entities being added by tick() in a single game tick for all the different entities with "KeepStockedBehaviour"
/// Also, T must be an Entity that has a constructor with no arguments
template <typename T> class KeepStockedBehaviour : public Behaviour {
    const int restockRate;
    int ticksUntilRestock;

  public:
    KeepStockedBehaviour(Entity &parent, int restockRate)
        : Behaviour("KeepStockedBehaviour", parent), restockRate(restockRate), ticksUntilRestock(restockRate) {}

    void tick() override {
        if (ticksUntilRestock == 0 && mParent.isInventoryEmpty()) {
            auto item = std::make_unique<T>();
            auto ID = item->mID;
            EntityManager::getInstance().addEntity(std::move(item));
            mParent.addToInventory(ID);
            ticksUntilRestock = restockRate;
        }
        if (ticksUntilRestock > 0)
            --ticksUntilRestock;
    }
};
