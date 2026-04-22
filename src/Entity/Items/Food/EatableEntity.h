#pragma once

#include "../../../Properties.h"
#include "../../Entity.h"

struct EatableEntity : Entity {
    EatableEntity(std::string ID, std::string name, std::string graphic, float hungerRestoration)
        : Entity(std::move(ID), std::move(name), std::move(graphic)) {
        addProperty(std::make_unique<EatableProperty>(hungerRestoration));
    }
};
