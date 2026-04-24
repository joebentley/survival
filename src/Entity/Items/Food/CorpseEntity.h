#pragma once

#include "EatableEntity.h"

struct CorpseEntity : EatableEntity {
    CorpseEntity(std::string ID, float hungerRestoration, const std::string &corpseOf, int weight);
};
