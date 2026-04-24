#include "HealingItemBehaviour.h"
#include "../../Entity/EntityManager.h"

HealingItemBehaviour::HealingItemBehaviour(Entity &parent, float healingAmount)
    : ApplyableBehaviour("HealingItemBehaviour", parent), healingAmount(healingAmount) {}

void HealingItemBehaviour::apply() {
    auto player = EntityManager::getInstance().getEntityByID("Player");
    player->addHealth(healingAmount);
    // destroy the parent entity once it is used
    player->removeFromInventory(mParent.mID);
    mParent.destroy();
}