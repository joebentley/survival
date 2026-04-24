#include "AppleEntity.h"

#include "../../../Properties.h"

AppleEntity::AppleEntity(std::string ID) : EatableEntity(std::move(ID), "Apple", "$[green]a", 0.5) {
    mShortDesc = SHORT_DESC;
    mLongDesc = LONG_DESC;
    addProperty(std::make_unique<PickuppableProperty>(1));
}
