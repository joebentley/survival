#include "CraftingMaterialProperty.h"

CraftingMaterialProperty::CraftingMaterialProperty(std::string type, float quality)
    : type(std::move(type)), quality(quality) {}

PROPERTY_SUBCLASS_TYPE_STRING(CraftingMaterial)
