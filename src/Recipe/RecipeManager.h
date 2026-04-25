#pragma once

#include "Recipe.h"
#include <vector>

struct RecipeManager {
    std::vector<std::unique_ptr<Recipe>> mRecipes;

    static RecipeManager &getInstance() {
        static RecipeManager instance;
        return instance;
    }

    RecipeManager();
    RecipeManager(const RecipeManager &) = delete;
    void operator=(const RecipeManager &) = delete;
};
