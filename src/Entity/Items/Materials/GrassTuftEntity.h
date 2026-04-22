#pragma once

#include "../../../Properties.h"
#include "../../Entity.h"

struct GrassTuftEntity : Entity {
    const std::string SHORT_DESC = "A tuft of dry grass";
    const std::string LONG_DESC = "A tuft of dry grass, very useful";

    explicit GrassTuftEntity(std::string ID = "") : Entity(std::move(ID), "Tuft of grass", "$[grasshay]$(approx)") {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<CraftingMaterialProperty>("grass", 1));
    }
};