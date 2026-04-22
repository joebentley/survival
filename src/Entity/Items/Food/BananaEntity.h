#pragma once

#include "EatableEntity.h"

struct BananaEntity : EatableEntity {
    const std::string SHORT_DESC = "A yellow fruit found in the jungle.";
    const std::string LONG_DESC =
        "This fruit was discovered in . They were brought west by Arab conquerors in 327 B.C.";

    explicit BananaEntity(std::string ID = "") : EatableEntity(std::move(ID), "Banana", "$[yellow]b", 0.5) {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};
