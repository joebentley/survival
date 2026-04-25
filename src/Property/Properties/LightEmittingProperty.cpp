#include "LightEmittingProperty.h"
#include "../../Entity/Entity.h"

LightEmittingProperty::LightEmittingProperty(Entity *parent, int radius, Color color)
    : mParent(parent), mRadius(radius), mColor(color) {}
LightEmittingProperty::LightEmittingProperty(Entity *parent, int radius)
    : mParent(parent), mRadius(radius), mColor(Color{}) {}

int LightEmittingProperty::getRadius() const { return mRadius; }

void LightEmittingProperty::setRadius(int radius) { mRadius = radius; }

bool LightEmittingProperty::isEnabled() const {
    // Don't be enabled if just sitting in someone's inventory
    return !mParent->mIsInAnInventory || mParent->mIsEquipped;
}

Color LightEmittingProperty::getColor() const { return mColor; }

void LightEmittingProperty::setColor(Color color) { mColor = color; }

PROPERTY_SUBCLASS_TYPE_STRING(LightEmitting)
