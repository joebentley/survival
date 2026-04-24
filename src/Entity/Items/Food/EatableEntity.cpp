#include "EatableEntity.h"

#include "../../../Properties.h"

EatableEntity::EatableEntity(std::string ID, std::string name, std::string graphic, float hungerRestoration)
    : Entity(std::move(ID), std::move(name), std::move(graphic)) {
    addProperty(std::make_unique<EatableProperty>(hungerRestoration));
}