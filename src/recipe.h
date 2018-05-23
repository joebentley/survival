#ifndef RECIPE_H_
#define RECIPE_H_

#include <vector>
#include <string>
#include "entity.h"
#include "entities.h"

struct Recipe;
struct RecipeManager {
    // TODO change to unique_ptr
    std::vector<std::shared_ptr<Recipe>> mRecipes;

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
        int mQuantity;
        std::string mEntityType;
    };

    std::vector<Ingredient> mIngredients;
    std::string mNameOfProduct; // Name to show in crafting menu
    bool mGoesIntoInventory {true}; // If true, item will go into inventory, if not it will be built in the world
    Point mPointIfNotGoingIntoInventory;

    Recipe() : Recipe("") {}
    explicit Recipe(std::string nameOfProduct) : mNameOfProduct(std::move(nameOfProduct)) {}
    Recipe(const Recipe &other) = default;

    virtual void produce() {};
};

struct FireRecipe : Recipe {
    static int numProduced;

    FireRecipe() : Recipe("Fire") {
        mIngredients.emplace_back(Ingredient { 2, "wood" });
        mIngredients.emplace_back(Ingredient { 2, "grass" });
        mGoesIntoInventory = false;
    }

    void produce() override;
};

struct BandageRecipe : Recipe {
    static int numProduced;

    BandageRecipe() : Recipe("Bandage") {
        mIngredients.emplace_back(Ingredient { 1, "grass" });
    }

    void produce() override;
};

struct TorchRecipe : Recipe {
    static int numProduced;

    TorchRecipe() : Recipe("Torch") {
        mIngredients.emplace_back(Ingredient { 1, "wood" });
        mIngredients.emplace_back(Ingredient { 1, "grass" });
    }

    void produce() override;
};

struct BagRecipe : Recipe {
    BagRecipe() : Recipe("Bag") {
        mIngredients.emplace_back(Ingredient { 5, "grass" });
    }

    void produce() override;
};

#endif //RECIPE_H_
