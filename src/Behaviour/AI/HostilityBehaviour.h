#pragma once

#include "../../Entity/Entity.h"
#include "../Behaviour.h"
#include <stdexcept>

/// Chase and attack the player if in range
struct HostilityBehaviour : Behaviour {
    /// Initialize the behaviour. Will throw exception if entity has no "ChaseAndAttackBehaviour"
    /// \param parent parent entity of this behaviour
    /// \param range range in which to consider attacking
    /// \param hostility probability to start attacking
    HostilityBehaviour(Entity &parent, float range, float hostility)
        : Behaviour("HostilityBehaviour", parent), range(range), hostility(hostility) {
        if (!parent.hasBehaviour("ChaseAndAttackBehaviour"))
            throw std::invalid_argument("Error: HostilityBehaviour cannot be added to entity with ID " + parent.mID +
                                        " as it does not have a ChaseAndAttackBehaviour");
    }

    float range;
    float hostility; // in [0, 1]
    void tick() override;
};
