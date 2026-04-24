#include "BagEntity.h"

#include "../../Properties.h"

BagEntity::BagEntity(std::string ID) : Entity(std::move(ID), "Grass Bag", "$[green]$(Phi)") {
    mShortDesc = "This crude grass bag allows you to carry a few more items";
    addProperty(std::make_unique<PickuppableProperty>(1));
    addProperty(std::make_unique<EquippableProperty>(EquipmentSlot::BACK));
    addProperty(std::make_unique<AdditionalCarryWeightProperty>(20));
}
