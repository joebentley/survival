#ifndef SURVIVAL_PROPERTIES_H
#define SURVIVAL_PROPERTIES_H

#include "Property.h"

class MeleeWeaponDamageProperty : public Property {
public:
    explicit MeleeWeaponDamageProperty(int damage) : Property("MeleeWeaponDamage", damage) {}
};

#endif //SURVIVAL_PROPERTIES_H
