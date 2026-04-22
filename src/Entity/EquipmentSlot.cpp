#include "EquipmentSlot.h"

EquipmentSlot &operator++(EquipmentSlot &slot) {
    if (slot == EquipmentSlot::BACK) {
        slot = EquipmentSlot::HEAD;
        return slot;
    }

    slot = static_cast<EquipmentSlot>(static_cast<int>(slot) + 1);
    return slot;
}

EquipmentSlot &operator--(EquipmentSlot &slot) {
    if (slot == EquipmentSlot::HEAD) {
        slot = EquipmentSlot::BACK;
        return slot;
    }

    slot = static_cast<EquipmentSlot>(static_cast<int>(slot) - 1);
    return slot;
}
