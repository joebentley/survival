#pragma once

#include "../Behaviour.h"

/// This behaviour causes the parent entity to wander aimlessly in every direction
struct WanderBehaviour : Behaviour {
    explicit WanderBehaviour(Entity &parent) : Behaviour("WanderBehaviour", parent) {}
    void tick() override;
};
