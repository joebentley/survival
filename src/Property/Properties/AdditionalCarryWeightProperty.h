#pragma once

#include "../Property.h"

class AdditionalCarryWeightProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(AdditionalCarryWeight)

    explicit AdditionalCarryWeightProperty(int additionalCarryWeight);

    int additionalCarryWeight;
};
