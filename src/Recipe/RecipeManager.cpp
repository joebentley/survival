#include "RecipeManager.h"

#include "Recipes/BagRecipe.h"
#include "Recipes/BandageRecipe.h"
#include "Recipes/FireRecipe.h"
#include "Recipes/TorchRecipe.h"

RecipeManager::RecipeManager() {
    mRecipes.emplace_back(std::make_unique<FireRecipe>());
    mRecipes.emplace_back(std::make_unique<BandageRecipe>());
    mRecipes.emplace_back(std::make_unique<TorchRecipe>());
    mRecipes.emplace_back(std::make_unique<BagRecipe>());
}
