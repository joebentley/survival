#include "BandageEntity.h"

#include "../../../Behaviour/Item/HealingItemBehaviour.h"
#include "../../../Property/Properties/PickuppableProperty.h"

BandageEntity::BandageEntity(std::string ID) : Entity(std::move(ID), "Bandage", "$[white]~") {
    mShortDesc = SHORT_DESC;
    addProperty(std::make_unique<PickuppableProperty>(1));
    addBehaviour(std::make_unique<HealingItemBehaviour>(*this, 5));
}
