#pragma once

#include "EatableEntity.h"

struct AppleEntity : EatableEntity {
    const std::string SHORT_DESC = "A small, fist-sized fruit that is hopefully crispy and juicy";
    const std::string LONG_DESC = "This is a longer description of the apple";

    explicit AppleEntity(std::string ID = "") : EatableEntity(std::move(ID), "Apple", "$[green]a", 0.5) {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};