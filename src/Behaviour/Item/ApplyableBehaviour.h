#pragma once

#include "../Behaviour.h"

// TODO: This should be a property, but difficult due to virtual methods
/// Abstract base class to represent behaviours that have an apply method, for example items that can be used
struct ApplyableBehaviour : Behaviour {
    ApplyableBehaviour(std::string ID, Entity &parent) : Behaviour(std::move(ID), parent) {}

    virtual void apply() = 0;
};
