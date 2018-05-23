#include "behaviours.h"
#include "entities.h"
#include "world.h"
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
    auto& ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(mParent.mID);

    auto& player = *EntityManager::getInstance().getEntityByID("Player");
    Point posOffset;

    if (player.getPos().mX > mParent.getPos().mX)
        posOffset.mX = 1;
    else if (player.getPos().mX < mParent.getPos().mX)
        posOffset.mX = -1;
    if (player.getPos().mY > mParent.getPos().mY)
        posOffset.mY = 1;
    else if (player.getPos().mY < mParent.getPos().mY)
        posOffset.mY = -1;

    if (mParent.getPos() + posOffset != player.getPos()) {
        if (mParent.getPos().distanceTo(player.getPos()) > range) {
            double r = randDouble();
            if (r < unattachment) { // Stop attacking if far away
                mEnabled = false;
                if (mParent.getBehaviourByID("WanderBehaviour") != nullptr) {
                    mParent.getBehaviourByID("WanderBehaviour")->enable();
                }
                if (mParent.getBehaviourByID("WanderAttachBehaviour") != nullptr) {
                    auto& wanderAttach = dynamic_cast<WanderAttachBehaviour&>(*mParent.getBehaviourByID("WanderAttachBehaviour"));
                    wanderAttach.onlyWander = true; // Never attach to player anymore
                    wanderAttach.enable();
                }
                if (mParent.getBehaviourByID("HostilityBehaviour") != nullptr) {
                    mParent.getBehaviourByID("HostilityBehaviour")->enable();
                } else if (postHostility != 0) { // Don't bother adding hostility if it won't ever be triggered
                    mParent.addBehaviour(std::make_unique<HostilityBehaviour>(mParent, postHostilityRange, postHostility));
                }
            }
        }

        if (randDouble() < clinginess)
            mParent.moveTo(mParent.getPos() + posOffset);
        return;
    }

    int damage = mParent.rollDamage();
    player.mHp -= damage;

    NotificationMessageRenderer::getInstance().queueMessage(
            mParent.mGraphic + " " + mParent.mName + " $[white]hit you for ${black}$[red]" + std::to_string(damage));
}

void HostilityBehaviour::tick() {
    auto chaseAndAttack = mParent.getBehaviourByID("ChaseAndAttackBehaviour");
    auto player = EntityManager::getInstance().getEntityByID("Player");

    if (chaseAndAttack != nullptr && !chaseAndAttack->isEnabled() && player != nullptr
        && mParent.getPos().distanceTo(player->getPos()) < range && randDouble() < hostility)
    {
        // Disable wandering and wanderattach
        auto b = mParent.getBehaviourByID("WanderBehaviour");
        if (b != nullptr)
            b->disable();
        b = mParent.getBehaviourByID("WanderAttachBehaviour");
        if (b != nullptr)
            b->disable();
        NotificationMessageRenderer::getInstance().queueMessage("${black}The $[red]" + mParent.mName + " $[white]went $[red]feral!");
        chaseAndAttack->enable();
    }
}
