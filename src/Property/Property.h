#pragma once

#include <string>

class Property {
  public:
    virtual std::string getName() = 0;

    virtual ~Property() = default;
};

// These macros generate a method and static variable allowing the name of
// the Property to be looked up at both runtime and compile time

// In C++17 `name` could be an inline variable in the class definition, but
// in this project we use C++14

// Goes in class body
#define PROPERTY_SUBCLASS_BODY(propertyName)                                                                           \
    static std::string name;                                                                                           \
    virtual std::string getName() { return #propertyName; }

// Goes in cpp file
#define PROPERTY_SUBCLASS_TYPE_STRING(propertyName) std::string propertyName##Property::name = #propertyName;