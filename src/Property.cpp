#include "Property.h"

void Property::setValue(boost::any value) {
    mValue = value;
}

boost::any &Property::getValue() {
    return mValue;
}

std::string Property::getName() const {
    return mPropertyName;
}

void PropertiesManager::registerPropertyName(std::string propertyName) {
    mPropertyNamesRegistered.emplace(std::move(propertyName));
}

bool PropertiesManager::isPropertyRegistered(const std::string &propertyName) const {
    return mPropertyNamesRegistered.find(propertyName) != mPropertyNamesRegistered.cend();
}

PropertiesManager &PropertiesManager::getInstance() {
    static PropertiesManager instance;
    // Add all the property names
    instance.registerPropertyName("MeleeWeaponDamage");
    instance.registerPropertyName("Pickuppable");
    instance.registerPropertyName("Equippable");
    instance.registerPropertyName("CraftingMaterial");
    instance.registerPropertyName("AdditionalCarryWeight");
    instance.registerPropertyName("LightEmitting");
    instance.registerPropertyName("WaterContainer");
    return instance;
}
