#pragma once

#include "../Property.h"

class CraftingMaterialProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(CraftingMaterial)

    CraftingMaterialProperty(std::string type, float quality);

    std::string type;
    float quality;
};
