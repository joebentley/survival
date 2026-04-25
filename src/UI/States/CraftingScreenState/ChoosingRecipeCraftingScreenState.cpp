#include "ChoosingRecipeCraftingScreenState.h"

#include "../../../Recipe/RecipeManager.h"
#include "../../Screens/CraftingScreen.h"
#include "ChoosingIngredientCraftingScreenState.h"

void ChoosingRecipeCraftingScreenState::onEntry(CraftingScreen &screen) {
    screen.setLayer(CraftingScreen::CraftingLayer::RECIPE);
    mChosenRecipe = screen.getChosenRecipe();
}

std::unique_ptr<CraftingScreenState> ChoosingRecipeCraftingScreenState::handleInput(CraftingScreen &screen,
                                                                                    SDL_KeyboardEvent &e) {
    auto &rm = RecipeManager::getInstance();

    switch (e.key) {
    case SDLK_ESCAPE:
        screen.disable();
        return nullptr;
    case SDLK_J:
        if ((size_t)mChosenRecipe == rm.mRecipes.size() - 1)
            mChosenRecipe = 0;
        else
            mChosenRecipe++;

        screen.setChosenIngredient(0);
        screen.setChosenMaterial(0);
        screen.setChosenRecipe(mChosenRecipe);
        break;
    case SDLK_K:
        if (mChosenRecipe == 0)
            mChosenRecipe = (int)rm.mRecipes.size() - 1;
        else
            mChosenRecipe--;

        screen.setChosenIngredient(0);
        screen.setChosenMaterial(0);
        screen.setChosenRecipe(mChosenRecipe);
        break;
    case SDLK_L:
    case SDLK_RETURN:
        screen.setCurrentRecipe(std::make_unique<Recipe>(Recipe(*rm.mRecipes[mChosenRecipe])));
        return std::make_unique<ChoosingIngredientCraftingScreenState>();
    }
    return nullptr;
}
