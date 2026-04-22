#pragma once

#include "../../Behaviours.h"
#include "../Entity.h"

struct WolfEntity : Entity {
    explicit WolfEntity(std::string ID = "")
        : Entity(std::move(ID), "Wolf", "${black}$[red]W", 20.0f, 20.0f, 0.05f, 1, 4, 100) {
        addBehaviour(std::make_unique<WanderBehaviour>(*this));
        auto chaseAndAttack = std::make_unique<ChaseAndAttackBehaviour>(*this, 0.8f, 0.05f, 8, 8, 0.9f);
        chaseAndAttack->disable();
        addBehaviour(std::move(chaseAndAttack));
        addBehaviour(std::make_unique<HostilityBehaviour>(*this, 12, 0.95f));

        mShortDesc = "A terrifying looking beast!";
    }

    void destroy() override;
};
