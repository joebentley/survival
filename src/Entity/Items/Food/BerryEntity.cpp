#include "BerryEntity.h"

#include "../../../Properties.h"

BerryEntity::BerryEntity(std::string ID) : EatableEntity(std::move(ID), "Berry", "$[purple]$(male)", 0.5) {
    mShortDesc = SHORT_DESC;
    mLongDesc = LONG_DESC;
    addProperty(std::make_unique<PickuppableProperty>(1));
}