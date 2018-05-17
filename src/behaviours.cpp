#include "behaviours.h"
#include "entities.h"
#include "world.h"
#include <cstdlib>
#include <iostream>

void WanderBehaviour::tick() {
    Point p = parent.getPos();
    switch (rand() % 20) {
        case 0:
            p.x++;
            break;
        case 1:
            p.x--;
            break;
        case 2:
            p.y++;
            break;
        case 3:
            p.y--;
            break;
        case 4:
            p.x++;
            p.y++;
            break;
        case 5:
            p.x--;
            p.y++;
            break;
        case 6:
            p.x++;
            p.y--;
            break;
        case 7:
            p.x--;
            p.y--;
            break;
    }

    parent.moveTo(p);
}

void AttachmentBehaviour::tick() {
    float r = randFloat();

    if (!attached) {
        if (r < attachment) {
            PlayerEntity& p = dynamic_cast<PlayerEntity&>(*EntityManager::getInstance().getEntityByID("Player"));
            if (parent.getPos().distanceTo(p.getPos()) < 10) {
                std::cout << parent.name << " has attached itself to player!" << std::endl;
                attached = true;
            }
        }
        return;
    }

    PlayerEntity& p = dynamic_cast<PlayerEntity&>(*EntityManager::getInstance().getEntityByID("Player"));

    // Don't follow too closely
    auto pos = parent.getPos();
    if (pos.distanceTo(p.getPos()) > 2 && r < clinginess) {
        if (pos.x < p.getPos().x)
            pos.x++;
        else if (pos.x > p.getPos().x)
            pos.x--;
        if (pos.y < p.getPos().y)
            pos.y++;
        else if (pos.y > p.getPos().y)
            pos.y--;
    }

    parent.moveTo(pos);

    if (r < unattachment) {
        std::cout << parent.name << " has detached itself from player!" << std::endl;
        attached = false;
        return;
    }
}

void ChaseAndAttackBehaviour::tick() {
    auto& ui = dynamic_cast<StatusUIEntity&>(*EntityManager::getInstance().getEntityByID("StatusUI"));
    ui.setAttackTarget(std::make_shared<Entity>(parent));

    auto& player = *EntityManager::getInstance().getEntityByID("Player");
    Point posOffset;

    if (player.getPos().x > parent.getPos().x)
        posOffset.x = 1;
    else if (player.getPos().x < parent.getPos().x)
        posOffset.x = -1;
    if (player.getPos().y > parent.getPos().y)
        posOffset.y = 1;
    else if (player.getPos().y < parent.getPos().y)
        posOffset.y = -1;

    if (parent.getPos() + posOffset != player.getPos()) {
        if (parent.getPos().distanceTo(player.getPos()) > range) {
            float r = randFloat();
            if (r < unattachment) { // Stop attacking if far away
                enabled = false;
                if (parent.getBehaviourByID("WanderBehaviour") != nullptr) {
                    parent.getBehaviourByID("WanderBehaviour")->enable();
                }
                if (parent.getBehaviourByID("WanderAttachBehaviour") != nullptr) {
                    auto& wanderAttach = dynamic_cast<WanderAttachBehaviour&>(*parent.getBehaviourByID("WanderAttachBehaviour"));
                    wanderAttach.onlyWander = true; // Never attach to player anymore
                    wanderAttach.enable();
                }
                if (parent.getBehaviourByID("HostilityBehaviour") != nullptr) {
                    parent.getBehaviourByID("HostilityBehaviour")->enable();
                } else if (postHostility != 0) { // Don't bother adding hostility if it won't ever be triggered
                    std::shared_ptr<Behaviour> hostilityBehaviour =
                            std::make_shared<HostilityBehaviour>(parent, postHostilityRange, postHostility);
                    parent.addBehaviour(hostilityBehaviour);
                }
            }
        }

        if (randFloat() < clinginess)
            parent.moveTo(parent.getPos() + posOffset);
        return;
    }

    int damage = parent.rollDamage();
    player.hp -= damage;
    std::cout << parent.name << " hit player with " << parent.hitTimes << "d" << parent.hitAmount << " for " << damage << "\n";
}

void HostilityBehaviour::tick() {
    auto chaseAndAttack = parent.getBehaviourByID("ChaseAndAttackBehaviour");
    auto player = EntityManager::getInstance().getEntityByID("Player");

    if (chaseAndAttack != nullptr && !chaseAndAttack->isEnabled() && player != nullptr
        && parent.getPos().distanceTo(player->getPos()) < range && randFloat() < hostility)
    {
        std::cout << parent.name << " became hostile!\n";
        chaseAndAttack->enable();
    }
}
