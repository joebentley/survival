#include "behaviours.h"
#include "entities.h"
#include "world.h"
#include <cstdlib>
#include <iostream>

void WanderBehaviour::tick() {
    switch (rand() % 20) {
        case 0:
            parent.pos.x++;
            break;
        case 1:
            parent.pos.x--;
            break;
        case 2:
            parent.pos.y++;
            break;
        case 3:
            parent.pos.y--;
            break;
        case 4:
            parent.pos.x++;
            parent.pos.y++;
            break;
        case 5:
            parent.pos.x--;
            parent.pos.y++;
            break;
        case 6:
            parent.pos.x++;
            parent.pos.y--;
            break;
        case 7:
            parent.pos.x--;
            parent.pos.y--;
            break;
    }
}

void AttachmentBehaviour::tick() {
    float r = randFloat();

    if (!attached) {
        if (r < attachment) {
            PlayerEntity& p = dynamic_cast<PlayerEntity&>(*parent.manager.getEntityByTag("Player"));
            if (parent.pos.distanceTo(p.pos) < 10) {
                std::cout << parent.name << " has attached itself to player!" << std::endl;
                attached = true;
            }
        }
        return;
    }

    PlayerEntity& p = dynamic_cast<PlayerEntity&>(*parent.manager.getEntityByTag("Player"));

    // Don't follow too closely
    if (parent.pos.distanceTo(p.pos) > 2 && r < clinginess) {
        if (parent.pos.x < p.pos.x)
            parent.pos.x++;
        else if (parent.pos.x > p.pos.x)
            parent.pos.x--;
        if (parent.pos.y < p.pos.y)
            parent.pos.y++;
        else if (parent.pos.y > p.pos.y)
            parent.pos.y--;
    }

    if (r < unattachment) {
        std::cout << parent.name << " has detached itself from player!" << std::endl;
        attached = false;
        return;
    }
}

void ChaseAndAttackBehaviour::tick() {
    auto& player = *parent.manager.getEntityByTag("Player");
    Point posOffset;

    if (player.pos.x > parent.pos.x)
        posOffset.x = 1;
    else if (player.pos.x < parent.pos.x)
        posOffset.x = -1;
    if (player.pos.y > parent.pos.y)
        posOffset.y = 1;
    else if (player.pos.y < parent.pos.y)
        posOffset.y = -1;

    auto entitiesInSquare = parent.manager.getEntitiesAtPos(parent.pos + posOffset);

    if (entitiesInSquare.empty() || entitiesInSquare[0]->tag != "Player") {
        if (parent.pos.distanceTo(player.pos) > range) {
            float r = randFloat();
            if (r < unattachment) { // Stop attacking if far away
                enabled = false;
                if (parent.getBehaviourByID("WanderBehaviour") != nullptr) {
                    parent.getBehaviourByID("WanderBehaviour")->enabled = true;
                }
                if (parent.getBehaviourByID("WanderAttachBehaviour") != nullptr) {
                    auto& wanderAttach = dynamic_cast<WanderAttachBehaviour&>(*parent.getBehaviourByID("WanderAttachBehaviour"));
                    wanderAttach.onlyWander = true; // Never attach to player anymore
                    wanderAttach.enabled = true;
                }
                if (parent.getBehaviourByID("HostilityBehaviour") != nullptr) {
                    parent.getBehaviourByID("HostilityBehaviour")->enabled = true;
                } else if (postHostility != 0) { // Don't bother adding hostility if it won't ever be triggered
                    std::shared_ptr<Behaviour> hostilityBehaviour =
                            std::make_shared<HostilityBehaviour>(parent, postHostilityRange, postHostility);
                    parent.addBehaviour(hostilityBehaviour);
                }
            }
        }

        if (randFloat() < clinginess)
            parent.pos += posOffset;
        return;
    }

    int damage = parent.rollDamage();
    player.hp -= damage;
    std::cout << parent.name << " hit player with " << parent.hitTimes << "d" << parent.hitAmount << " for " << damage << "\n";

}

void HostilityBehaviour::tick() {
    auto chaseAndAttack = parent.getBehaviourByID("ChaseAndAttackBehaviour");
    auto player = parent.manager.getEntityByTag("Player");

    if (chaseAndAttack != nullptr && !chaseAndAttack->enabled && player != nullptr
        && parent.pos.distanceTo(player->pos) < range && randFloat() < hostility)
    {
        std::cout << parent.name << " became hostile!\n";
        chaseAndAttack->enabled = true;
    }
}
