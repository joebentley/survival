#pragma once

#include "../Behaviour.h"

/// Chase and attack the entity with ID "Player". If parent has a "WanderBehaviour" then that will enable after
/// attacking, if it has a "WanderAttachBehaviour" it will wander but not attach anymore, if it has "HostilityBehaviour"
/// then re-enable that, otherwise if postHostility != 0 it will create a new "HostilityBehaviour" with parameters
/// postHostilityRange and postHostility
struct ChaseAndAttackBehaviour : Behaviour {
    /// Initialize the behaviour
    /// \param parent parent of this behaviour
    /// \param clinginess probability of moving towards the player on tick
    /// \param unattachment probability to stop attacking if out of range
    /// \param range distance at which we might stop attacking and chasing player
    /// \param postHostilityRange range at which to be hostile to player within after stopping attacking
    /// \param postHostility probability of becoming hostile after stopping attacking
    ChaseAndAttackBehaviour(Entity &parent, float clinginess, float unattachment, float range, float postHostilityRange,
                            float postHostility)
        : Behaviour("ChaseAndAttackBehaviour", parent), clinginess(clinginess), unattachment(unattachment),
          range(range), postHostilityRange(postHostilityRange), postHostility(postHostility) {}

    float clinginess;
    float unattachment;
    float range;
    float postHostilityRange;
    float postHostility;
    void tick() override;
};
