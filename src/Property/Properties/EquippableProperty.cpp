#include "EquippableProperty.h"

EquippableProperty::EquippableProperty(std::vector<EquipmentSlot> equippableSlots)
    : m_equippableSlots(std::move(equippableSlots)) {}

EquippableProperty::EquippableProperty(EquipmentSlot equippableSlot)
    : m_equippableSlots(std::vector<EquipmentSlot>(1, equippableSlot)) {}

const std::vector<EquipmentSlot> &EquippableProperty::getEquippableSlots() const { return m_equippableSlots; }

bool EquippableProperty::isEquippableInSlot(EquipmentSlot slot) const {
    return std::find(m_equippableSlots.cbegin(), m_equippableSlots.cend(), slot) != m_equippableSlots.cend();
}

PROPERTY_SUBCLASS_TYPE_STRING(Equippable)
