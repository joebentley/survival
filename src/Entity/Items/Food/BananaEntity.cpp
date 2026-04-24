#include "BananaEntity.h"

#include "../../../Properties.h"

BananaEntity::BananaEntity(std::string ID) : EatableEntity(std::move(ID), "Banana", "$[yellow]b", 0.5) {
    mShortDesc = SHORT_DESC;
    mLongDesc = LONG_DESC;
    addProperty(std::make_unique<PickuppableProperty>(1));
}