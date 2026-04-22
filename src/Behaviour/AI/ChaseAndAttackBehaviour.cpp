#include "ChaseAndAttackBehaviour.h"

#include "../../Entity/UI/StatusUIEntity.h"
#include "HostilityBehaviour.h"
#include "WanderAttachBehaviour.h"

void ChaseAndAttackBehaviour::tick() {
    // get the status UI and set the player's attack target to the parent entity
    auto &ui = dynamic_cast<StatusUIEntity &>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(mParent.mID);

    auto &player = *EntityManager::getInstance().getEntityByID("Player");
    Point posOffset;

    // move towards the player by 1 step
    if (player.getPos().mX > mParent.getPos().mX)
        posOffset.mX = 1;
    else if (player.getPos().mX < mParent.getPos().mX)
        posOffset.mX = -1;
    if (player.getPos().mY > mParent.getPos().mY)
        posOffset.mY = 1;
    else if (player.getPos().mY < mParent.getPos().mY)
        posOffset.mY = -1;

    // if we are more than 1 square away from the player
    if (mParent.getPos() + posOffset != player.getPos()) {
        // if we are more than `range` away from the player
        if (mParent.getPos().distanceTo(player.getPos()) > range) {
            double r = randDouble();
            // if we roll less than the unattachment probability
            if (r < unattachment) {
                // disable this behaviour
                mEnabled = false;

                // re-enable wandering
                if (mParent.getBehaviourByID("WanderBehaviour") != nullptr) {
                    mParent.getBehaviourByID("WanderBehaviour")->enable();
                }
                // re-enable WanderAttach but don't attach anymore
                if (mParent.getBehaviourByID("WanderAttachBehaviour") != nullptr) {
                    auto &wanderAttach =
                        dynamic_cast<WanderAttachBehaviour &>(*mParent.getBehaviourByID("WanderAttachBehaviour"));
                    wanderAttach.onlyWander = true; // Never attach to player anymore
                    wanderAttach.enable();
                }
                // re-enable the hostility behaviour
                if (mParent.getBehaviourByID("HostilityBehaviour") != nullptr) {
                    mParent.getBehaviourByID("HostilityBehaviour")->enable();
                }
                // else add a hostility behaviour
                else if (postHostility != 0) { // Don't bother adding hostility if it won't ever be triggered
                    mParent.addBehaviour(
                        std::make_unique<HostilityBehaviour>(mParent, postHostilityRange, postHostility));
                }
            }
        }

        // if we roll less than clinginess, move towards the player
        if (randDouble() < clinginess)
            mParent.moveTo(mParent.getPos() + posOffset);
        return;
    }

    // Attack the player
    int damage = mParent.rollDamage();
    player.mHp -= damage;

    // Send a notification logging the attack
    NotificationMessageRenderer::getInstance().queueMessage(
        mParent.mGraphic + " " + mParent.mName + " $[white]hit you for ${black}$[red]" + std::to_string(damage));
}
