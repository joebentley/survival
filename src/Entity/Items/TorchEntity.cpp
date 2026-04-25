#include "TorchEntity.h"

#include "../../Property/Properties/EquippableProperty.h"
#include "../../Property/Properties/LightEmittingProperty.h"
#include "../../Property/Properties/PickuppableProperty.h"

TorchEntity::TorchEntity(std::string ID) : Entity(std::move(ID), "Torch", "$[red]$(up)") {
    addProperty(std::make_unique<PickuppableProperty>(1));
    addProperty(std::make_unique<LightEmittingProperty>(this, 4));
    addProperty(std::make_unique<EquippableProperty>(
        std::vector<EquipmentSlot>{EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND}));

    mShortDesc = "Can be equipped to produce light and some heat.";
}
