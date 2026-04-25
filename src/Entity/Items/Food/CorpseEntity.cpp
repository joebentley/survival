#include "CorpseEntity.h"

#include "../../../Property/Properties/PickuppableProperty.h"

CorpseEntity::CorpseEntity(std::string ID, float hungerRestoration, const std::string &corpseOf, int weight)
    : EatableEntity(std::move(ID), "Corpse of " + corpseOf, "${black}$[red]x", hungerRestoration) {
    addProperty(std::make_unique<PickuppableProperty>(weight));
}
