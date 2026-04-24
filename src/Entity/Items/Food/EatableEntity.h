#pragma once

#include "../../Entity.h"

struct EatableEntity : Entity {
    EatableEntity(std::string ID, std::string name, std::string graphic, float hungerRestoration);
};
