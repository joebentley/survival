#pragma once

#include "../../../Behaviour/Item/HealingItemBehaviour.h"
#include "../../../Properties.h"
#include "../../Entity.h"

struct BandageEntity : Entity {
    const std::string SHORT_DESC = "A rudimentary bandage made of grass";

    explicit BandageEntity(std::string ID = "") : Entity(std::move(ID), "Bandage", "$[white]~") {
        mShortDesc = SHORT_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
        addBehaviour(std::make_unique<HealingItemBehaviour>(*this, 5));
    }
};