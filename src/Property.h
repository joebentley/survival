#ifndef SURVIVAL_PROPERTY_H
#define SURVIVAL_PROPERTY_H

#include <boost/any.hpp>
#include <string>
#include <set>

class PropertiesManager {
public:
    static PropertiesManager& getInstance() {
        static PropertiesManager instance;
        return instance;
    }

    PropertiesManager() = default;
    PropertiesManager(const PropertiesManager&) = delete;
    void operator=(const PropertiesManager&) = delete;

    void registerPropertyName(std::string propertyName);
    bool isPropertyRegistered(std::string propertyName) const;

private:
    std::set<std::string> mPropertyNamesRegistered;
};

class Property {
public:
    Property(std::string propertyName, boost::any value) : mPropertyName(std::move(propertyName)), mValue(std::move(value)) {
        PropertiesManager::getInstance().registerPropertyName(mPropertyName);
    }

    std::string getName() const;
    void setValue(boost::any value);
    boost::any &getValue();

private:
    std::string mPropertyName;
    boost::any mValue;
};


#endif //SURVIVAL_PROPERTY_H
