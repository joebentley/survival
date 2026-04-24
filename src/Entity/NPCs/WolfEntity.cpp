#include "WolfEntity.h"
#include "../../Behaviour/AI/ChaseAndAttackBehaviour.h"
#include "../../Behaviour/AI/HostilityBehaviour.h"
#include "../../Behaviour/AI/WanderBehaviour.h"
#include "../EntityManager.h"
#include "../Items/Food/CorpseEntity.h"

WolfEntity::WolfEntity(std::string ID)
    : Entity(std::move(ID), "Wolf", "${black}$[red]W", 20.0f, 20.0f, 0.05f, 1, 4, 100) {
    addBehaviour(std::make_unique<WanderBehaviour>(*this));
    auto chaseAndAttack = std::make_unique<ChaseAndAttackBehaviour>(*this, 0.8f, 0.05f, 8, 8, 0.9f);
    chaseAndAttack->disable();
    addBehaviour(std::move(chaseAndAttack));
    addBehaviour(std::make_unique<HostilityBehaviour>(*this, 12, 0.95f));

    mShortDesc = "A terrifying looking beast!";
}

void WolfEntity::destroy() {
    auto corpse = std::make_unique<CorpseEntity>(mID + "corpse", 0.4, mName, 100);
    corpse->setPos(getPos());
    EntityManager::getInstance().addEntity(std::move(corpse));
}