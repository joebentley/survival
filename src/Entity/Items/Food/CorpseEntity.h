#pragma once

#include "EatableEntity.h"

struct CorpseEntity : EatableEntity {
    CorpseEntity(std::string ID, float hungerRestoration, const std::string &corpseOf, int weight)
        : EatableEntity(std::move(ID), "Corpse of " + corpseOf, "${black}$[red]x", hungerRestoration) {
        addProperty(std::make_unique<PickuppableProperty>(weight));
    }
};
