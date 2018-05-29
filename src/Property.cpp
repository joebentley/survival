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

bool PropertiesManager::isPropertyRegistered(std::string propertyName) const {
    return mPropertyNamesRegistered.find(std::move(propertyName)) != mPropertyNamesRegistered.cend();
}
