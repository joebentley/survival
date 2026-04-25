#pragma once

#include "../Property.h"
#include <vector>

enum class EquipmentSlot;
class EquippableProperty : public Property {
  public:
    PROPERTY_SUBCLASS_BODY(Equippable)

    explicit EquippableProperty(std::vector<EquipmentSlot> equippableSlots);

    explicit EquippableProperty(EquipmentSlot equippableSlot);

    [[nodiscard]] const std::vector<EquipmentSlot> &getEquippableSlots() const;

    [[nodiscard]] bool isEquippableInSlot(EquipmentSlot slot) const;

  private:
    std::vector<EquipmentSlot> m_equippableSlots;
};
