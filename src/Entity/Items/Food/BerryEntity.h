#pragma once

#include "EatableEntity.h"

struct BerryEntity : EatableEntity {
    const std::string SHORT_DESC = "A purple berry";
    const std::string LONG_DESC = "";

    explicit BerryEntity(std::string ID = "") : EatableEntity(std::move(ID), "Berry", "$[purple]$(male)", 0.5) {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};