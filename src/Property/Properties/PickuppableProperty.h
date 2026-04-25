#pragma once

#include "../Property.h"

class PickuppableProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(Pickuppable)

    explicit PickuppableProperty(int weight = 1);

    int weight;
};
