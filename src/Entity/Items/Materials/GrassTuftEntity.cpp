#include "GrassTuftEntity.h"
#include "../../../Properties.h"

GrassTuftEntity::GrassTuftEntity(std::string ID) : Entity(std::move(ID), "Tuft of grass", "$[grasshay]$(approx)") {
    mShortDesc = SHORT_DESC;
    mLongDesc = LONG_DESC;
    addProperty(std::make_unique<PickuppableProperty>(1));
    addProperty(std::make_unique<CraftingMaterialProperty>("grass", 1));
}