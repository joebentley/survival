#pragma once

#include "../../../Properties.h"
#include "../../Entity.h"

struct TwigEntity : Entity {
    const std::string SHORT_DESC = "A thin, brittle twig";
    const std::string LONG_DESC = "It looks very useful! Who knows where it came from...";

    explicit TwigEntity(std::string ID = "") : Entity(std::move(ID), "Twig", "${black}$[brown]/") {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<CraftingMaterialProperty>("wood", 1));
        addProperty(std::make_unique<EquippableProperty>(
            std::vector<EquipmentSlot>{EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND}));
        //        addBehaviour(std::make_unique<MeleeWeaponBehaviour>(*this, 1));
        addProperty(std::make_unique<MeleeWeaponDamageProperty>(1));
    }
};