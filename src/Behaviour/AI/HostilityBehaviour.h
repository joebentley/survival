#pragma once

#include "../Behaviour.h"

/// Chase and attack the player if in range
struct HostilityBehaviour : Behaviour {
    /// Initialize the behaviour. Will throw exception if entity has no "ChaseAndAttackBehaviour"
    /// \param parent parent entity of this behaviour
    /// \param range range in which to consider attacking
    /// \param hostility probability to start attacking
    HostilityBehaviour(Entity &parent, float range, float hostility);

    float range;
    float hostility; // in [0, 1]
    void tick() override;
};
