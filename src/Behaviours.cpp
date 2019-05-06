#include "Behaviours.h"
#include "Entities.h"
#include "World.h"
#include "settings.h"
#include <cstdlib>
#include <iostream>

void WanderBehaviour::tick() {
    Point p = mParent.getPos();
    switch (rand() % 20) {
        case 0:
            p.mX++;
            break;
        case 1:
            p.mX--;
            break;
        case 2:
            p.mY++;
            break;
        case 3:
            p.mY--;
            break;
        case 4:
            p.mX++;
            p.mY++;
            break;
        case 5:
            p.mX--;
            p.mY++;
            break;
        case 6:
            p.mX++;
            p.mY--;
            break;
        case 7:
            p.mX--;
            p.mY--;
            break;
    }

    mParent.moveTo(p);
}

void AttachmentBehaviour::tick() {
    auto r = randDouble();

    if (!attached) {
        if (r < attachment) {
            PlayerEntity& p = dynamic_cast<PlayerEntity&>(*EntityManager::getInstance().getEntityByID("Player"));
            if (mParent.getPos().distanceTo(p.getPos()) < 10) {
                NotificationMessageRenderer::getInstance().queueMessage(mParent.mGraphic + "$[red]$(heart)$[white]$(dwarf)");
                attached = true;
            }
        }
        return;
    }

    PlayerEntity& p = dynamic_cast<PlayerEntity&>(*EntityManager::getInstance().getEntityByID("Player"));

    // Don't follow too closely
    auto pos = mParent.getPos();
    if (pos.distanceTo(p.getPos()) > 2 && r < clinginess) {
        if (pos.mX < p.getPos().mX)
            pos.mX++;
        else if (pos.mX > p.getPos().mX)
            pos.mX--;
        if (pos.mY < p.getPos().mY)
            pos.mY++;
        else if (pos.mY > p.getPos().mY)
            pos.mY--;
    }

    mParent.moveTo(pos);

    if (r < unattachment) {
        attached = false;
        return;
    }
}

void ChaseAndAttackBehaviour::tick() {
    // get the status UI and set the player's attack target to the parent entity
    auto& ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(mParent.mID);

    auto& player = *EntityManager::getInstance().getEntityByID("Player");
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
                    auto& wanderAttach = dynamic_cast<WanderAttachBehaviour&>(*mParent.getBehaviourByID("WanderAttachBehaviour"));
                    wanderAttach.onlyWander = true; // Never attach to player anymore
                    wanderAttach.enable();
                }
                // re-enable the hostility behaviour
                if (mParent.getBehaviourByID("HostilityBehaviour") != nullptr) {
                    mParent.getBehaviourByID("HostilityBehaviour")->enable();
                }
                // else add a hostility behaviour
                else if (postHostility != 0) { // Don't bother adding hostility if it won't ever be triggered
                    mParent.addBehaviour(std::make_unique<HostilityBehaviour>(mParent, postHostilityRange, postHostility));
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

void HostilityBehaviour::tick() {
    auto chaseAndAttack = mParent.getBehaviourByID("ChaseAndAttackBehaviour");
    auto player = EntityManager::getInstance().getEntityByID("Player");

    if (chaseAndAttack != nullptr && !chaseAndAttack->isEnabled() && player != nullptr &&
        randDouble() < hostility && mParent.getPos().distanceTo(player->getPos()) < range)
    {
        mParent.disableWanderBehaviours();

        // Send a notification notifying the player they're engaged in attack
        NotificationMessageRenderer::getInstance().queueMessage("${black}The $[red]" + mParent.mName + " $[white]went $[red]feral!");
        chaseAndAttack->enable();
    }
}

void SeekHomeBehaviour::tick() {
    if (homeTargetID.empty() && randDouble() < homeAttachmentProbability) {
        // nb: this is only the entities on the screens surrounding the player
        std::vector<Entity *> entities = EntityManager::getInstance().getEntitiesOnScreenAndSurroundingScreens();

        // filter for home entities that have the specified name and are in range
        entities.erase(std::remove_if(entities.begin(), entities.end(), [this](Entity *entity) {
            return !(entity->mName == homeName && entity->getPos().distanceTo(mParent.getPos()) < range);
        }), entities.end());

        // pick one of these home entities at random to set as target
        if (!entities.empty()) {
            homeTargetID = entities[rand() % entities.size()]->mID;
        
            if (_DEBUG)
                NotificationMessageRenderer::getInstance().queueMessage(
                    mParent.mGraphic + " (" + mParent.mID + ") chose home " + homeName + " (" + homeTargetID + ")");
        }
    }
    
    if (!homeTargetID.empty()) {
        mParent.disableWanderBehaviours();
        
        // move towards the chosen home entity
        Entity *homeTarget = EntityManager::getInstance().getEntityByID(homeTargetID);
        Point targetPos = homeTarget->getPos();
        
        // if we are at the target home, stop moving towards it
        if (targetPos == mParent.getPos()) {
            isInHome = true;
            
            // random chance to leave the home and start wandering again
            if (randDouble() < homeFlightProbability) {
                homeTargetID.clear();
                mParent.enableWanderBehaviours();
            }
        
            return;
        }
        
        isInHome = false;
        
        Point posOffset;
        
        if (targetPos.mX > mParent.getPos().mX) {
            posOffset.mX = 1;
        } else if (targetPos.mX < mParent.getPos().mX) {
            posOffset.mX = -1;
        }
        if (targetPos.mY > mParent.getPos().mY) {
            posOffset.mY = 1;
        } else if (targetPos.mY < mParent.getPos().mY) {
            posOffset.mY = -1;
        }
        
        mParent.moveTo(mParent.getPos() + posOffset);
    }
}
