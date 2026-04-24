#include "HostilityBehaviour.h"

#include "../../Entity/EntityManager.h"
#include "../../UI/NotificationMessageRenderer.h"
#include "../../utils.h"

void HostilityBehaviour::tick() {
    auto chaseAndAttack = mParent.getBehaviourByID("ChaseAndAttackBehaviour");
    auto player = EntityManager::getInstance().getEntityByID("Player");

    if (chaseAndAttack != nullptr && !chaseAndAttack->isEnabled() && player != nullptr && randDouble() < hostility &&
        mParent.getPos().distanceTo(player->getPos()) < range) {
        mParent.disableWanderBehaviours();

        // Send a notification notifying the player they're engaged in attack
        NotificationMessageRenderer::getInstance().queueMessage("${black}The $[red]" + mParent.mName +
                                                                " $[white]went $[red]feral!");
        chaseAndAttack->enable();
    }
}
