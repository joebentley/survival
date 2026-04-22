#pragma once
#include <vector>

/// Describes the different slots that equipment can be equipped in
enum class EquipmentSlot {
    HEAD,
    TORSO,
    LEGS,
    RIGHT_HAND,
    LEFT_HAND,
    FEET,
    BACK,
};

/// Get next equipment slot
EquipmentSlot &operator++(EquipmentSlot &slot);
/// Get previous equipment slot
EquipmentSlot &operator--(EquipmentSlot &slot);

/// Vector of all possible equipment slots (for iteration)
const std::vector<EquipmentSlot> EQUIPMENT_SLOTS{
    // NOLINT(cert-err58-cpp)
    EquipmentSlot::HEAD,      EquipmentSlot::TORSO, EquipmentSlot::LEGS, EquipmentSlot::RIGHT_HAND,
    EquipmentSlot::LEFT_HAND, EquipmentSlot::FEET,  EquipmentSlot::BACK};

/// Convert equipment slot to string
inline std::string slotToString(EquipmentSlot slot) {
    if (slot == EquipmentSlot::HEAD)
        return "Head";
    if (slot == EquipmentSlot::TORSO)
        return "Torso";
    if (slot == EquipmentSlot::LEGS)
        return "Legs";
    if (slot == EquipmentSlot::RIGHT_HAND)
        return "Right hand";
    if (slot == EquipmentSlot::LEFT_HAND)
        return "Left hand";
    if (slot == EquipmentSlot::FEET)
        return "Feet";
    if (slot == EquipmentSlot::BACK)
        return "Back";
    return "";
}
