#pragma once

#include "Entity.h"

struct ChestEntity : Entity {
    const std::string SHORT_DESC = "A heavy wooden chest";
    const std::string LONG_DESC = "This chest is super heavy";

    explicit ChestEntity(std::string ID = "") : Entity(std::move(ID), "Chest", "${black}$[brown]$(accentAE)") {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;

        // TODO: allow player to place items in chest
    }
};