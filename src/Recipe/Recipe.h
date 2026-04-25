#pragma once

#include "../Point.h"
#include <algorithm>
#include <vector>

struct Recipe {
    struct Ingredient {
        int mQuantity;
        std::string mEntityType;
    };

    std::vector<Ingredient> mIngredients;
    std::string mNameOfProduct;    // Name to show in crafting menu
    bool mGoesIntoInventory{true}; // If true, item will go into inventory, if not it will be built in the world
    Point mPointIfNotGoingIntoInventory;

    Recipe() : Recipe("") {}
    explicit Recipe(std::string nameOfProduct) : mNameOfProduct(std::move(nameOfProduct)) {}
    Recipe(const Recipe &other) = default;

    virtual void produce() {};

    virtual ~Recipe() = default;
};
