#ifndef SURVIVAL_PROPERTIES_H
#define SURVIVAL_PROPERTIES_H

#include "Property.h"

class MeleeWeaponDamageProperty : public Property {
public:
    explicit MeleeWeaponDamageProperty(int damage) : Property("MeleeWeaponDamage", damage) {}
};

class PickuppableProperty : public Property {
public:
    explicit PickuppableProperty(int weight = 1) : Property("Pickuppable", weight) {}
};

class EquippableProperty : public Property {
public:
    explicit EquippableProperty(std::vector<EquipmentSlot> equippableSlots)
            : Property("Equippable", Equippable(equippableSlots)) {}
    explicit EquippableProperty(EquipmentSlot equippableSlot)
            : Property("Equippable", Equippable(equippableSlot)) {}

    class Equippable {
    public:
        explicit Equippable(std::vector<EquipmentSlot> equippableSlots) : mEquippableSlots(std::move(equippableSlots)) {}

        explicit Equippable(EquipmentSlot equippableSlot) : mEquippableSlots(std::vector<EquipmentSlot>(1, equippableSlot)) {}

        const std::vector<EquipmentSlot> &getEquippableSlots() const {
            return mEquippableSlots;
        }

        bool isEquippableInSlot(EquipmentSlot slot) const {
            return std::find(mEquippableSlots.cbegin(), mEquippableSlots.cend(), slot) != mEquippableSlots.cend();
        };

    private:
        std::vector<EquipmentSlot> mEquippableSlots;
    };
};

class CraftingMaterialProperty : public Property {
public:
    CraftingMaterialProperty(std::string materialType, float materialQuality)
            : Property("CraftingMaterial", Data {std::move(materialType), materialQuality}) {}

    struct Data {
        std::string type;
        float quality;
    };
};

class AdditionalCarryWeightProperty : public Property {
public:
    explicit AdditionalCarryWeightProperty(int additionalCarryWeight)
            : Property("AdditionalCarryWeight", additionalCarryWeight) {}
};

class LightEmittingProperty : public Property {
public:
    LightEmittingProperty(Entity *parent, int radius, Color color)
            : Property("LightEmitting", Light(parent, radius, color)) {}

    LightEmittingProperty(Entity *parent, int radius)
            : Property("LightEmitting", Light(parent, radius)) {}

    class Light {
    public:
        Light(Entity *parent, int radius, Color color) : mParent(parent), mRadius(radius), mColor(color) {}
        Light(Entity *parent, int radius) : mParent(parent), mRadius(radius), mColor(Color{}) {}

        int getRadius() const {
            return mRadius;
        }

        void setRadius(int radius) {
            mRadius = radius;
        }

        bool isEnabled() const {
            // Don't be enabled if just sitting in someone's inventory
            return !mParent->mIsInAnInventory || mParent->mIsEquipped;
        }

        Color getColor() const {
            return mColor;
        }

        void setColor(Color color) {
            mColor = color;
        }

    private:
        Entity *mParent;
        int mRadius;
        Color mColor;
    };
};

#endif //SURVIVAL_PROPERTIES_H
