#pragma once

#include "../../Behaviours.h"
#include "../Entity.h"

struct CatEntity : Entity {
    explicit CatEntity(std::string ID = "")
        : Entity(std::move(ID), "Cat", "$[yellow]c", 10.0f, 10.0f, 0.05f, 1, 2, 100) {
        auto wanderAttach = std::make_unique<WanderAttachBehaviour>(*this, 0.5f, 0.7f, 0.05f);
        auto chaseAndAttack = std::make_unique<ChaseAndAttackBehaviour>(*this, 0.8f, 0.1f, 8.0f, 8.0f, 0.9f);
        chaseAndAttack->disable();
        addBehaviour(std::move(wanderAttach));
        addBehaviour(std::move(chaseAndAttack));
    }

    void destroy() override;
};