#ifndef RECIPE_H_
#define RECIPE_H_

#include <vector>
#include <string>
#include "entity.h"
#include "entities.h"

struct Recipe;
struct RecipeManager {
    std::vector<std::shared_ptr<Recipe>> recipes;

    static RecipeManager& getInstance() {
        static RecipeManager instance;
        return instance;
    }

    RecipeManager();
    RecipeManager(const RecipeManager&) = delete;
    void operator=(const RecipeManager&) = delete;
};

struct Recipe {
    struct Ingredient {
        int quantity;
        std::string entityType;
    };

    std::vector<Ingredient> ingredients;
    std::string nameOfProduct; // Name to show in crafting menu
    bool goesIntoInventory {true}; // If true, item will go into inventory, if not it will be built in the world
    Point pointIfNotGoingIntoInventory;

    Recipe() : Recipe("") {}
    explicit Recipe(std::string nameOfProduct) : nameOfProduct(std::move(nameOfProduct)) {}
    Recipe(const Recipe &other) = default;

    virtual void produce() {};
};

struct FireRecipe : Recipe {
    static int numProduced;

    FireRecipe() : Recipe("Fire") {
        ingredients.emplace_back(Ingredient { 2, "wood" });
        ingredients.emplace_back(Ingredient { 2, "grass" });
        goesIntoInventory = false;
    }

    void produce() override;
};

struct BandageRecipe : Recipe {
    static int numProduced;

    BandageRecipe() : Recipe("Bandage") {
        ingredients.emplace_back(Ingredient { 1, "grass" });
    }

    void produce() override;
};

#endif //RECIPE_H_
