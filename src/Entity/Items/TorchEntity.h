#pragma once

#include "../../Properties.h"
#include "../Entity.h"

struct TorchEntity : Entity {
    explicit TorchEntity(std::string ID = "") : Entity(std::move(ID), "Torch", "$[red]$(up)") {
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<LightEmittingProperty>(this, 4));
        addProperty(std::make_unique<EquippableProperty>(
            std::vector<EquipmentSlot>{EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND}));

        mShortDesc = "Can be equipped to produce light and some heat.";
    }
};