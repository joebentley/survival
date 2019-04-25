#ifndef SURVIVAL_PROPERTY_H
#define SURVIVAL_PROPERTY_H

#include <any>
#include <string>
#include <set>

class PropertiesManager {
public:
    static PropertiesManager& getInstance();

    PropertiesManager() = default;
    PropertiesManager(const PropertiesManager&) = delete;
    void operator=(const PropertiesManager&) = delete;

    void registerPropertyName(std::string propertyName);
    bool isPropertyRegistered(const std::string &propertyName) const;

private:
    std::set<std::string> mPropertyNamesRegistered;
};

class Property {
public:
    Property(std::string propertyName, std::any value) : mPropertyName(std::move(propertyName)), mValue(std::move(value)) {}

    std::string getName() const;
    void setValue(std::any value);
    std::any &getValue();

private:
    std::string mPropertyName;
    std::any mValue;
};


#endif //SURVIVAL_PROPERTY_H
