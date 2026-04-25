#pragma once

#include "../Property.h"

class MeleeWeaponDamageProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(MeleeWeaponDamage)

    explicit MeleeWeaponDamageProperty(int damage);

    int damage;
};
