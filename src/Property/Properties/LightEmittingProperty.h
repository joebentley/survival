#pragma once

#include "../../Color.h"
#include "../Property.h"

struct Entity;
class LightEmittingProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(LightEmitting)

    LightEmittingProperty(Entity *parent, int radius, Color color);
    LightEmittingProperty(Entity *parent, int radius);

    [[nodiscard]] bool isEnabled() const;
    [[nodiscard]] int getRadius() const;
    void setRadius(int radius);
    [[nodiscard]] Color getColor() const;
    void setColor(Color color);

  private:
    Entity *mParent;
    int mRadius;
    Color mColor;
};
