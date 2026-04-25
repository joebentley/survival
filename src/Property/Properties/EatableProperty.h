#pragma once

#include "../Property.h"

class EatableProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(Eatable)

    explicit EatableProperty(float hungerRestoration);

    float hungerRestoration;
};
