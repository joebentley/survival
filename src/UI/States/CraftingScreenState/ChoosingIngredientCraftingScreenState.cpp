#include "ChoosingIngredientCraftingScreenState.h"

#include "../../Screens/CraftingScreen.h"
#include "ChoosingBuildPositionCraftingScreenState.h"
#include "ChoosingMaterialCraftingScreenState.h"
#include "ChoosingRecipeCraftingScreenState.h"

void ChoosingIngredientCraftingScreenState::onEntry(CraftingScreen &screen) {
    mChosenIngredient = screen.getChosenIngredient();
    screen.setLayer(CraftingScreen::CraftingLayer::INGREDIENT);
}

std::unique_ptr<CraftingScreenState> ChoosingIngredientCraftingScreenState::handleInput(CraftingScreen &screen,
                                                                                        SDL_KeyboardEvent &e) {
    auto &rm = RecipeManager::getInstance();
    switch (e.key) {
    case SDLK_ESCAPE:
        screen.reset();
        screen.disable();
        break;
    case SDLK_J:
        if ((size_t)mChosenIngredient == rm.mRecipes[screen.getChosenRecipe()]->mIngredients.size())
            mChosenIngredient = 0;
        else
            mChosenIngredient++;
        screen.setChosenIngredient(mChosenIngredient);
        screen.setChosenMaterial(0);
        break;
    case SDLK_K:
        if (mChosenIngredient == 0)
            mChosenIngredient = (int)rm.mRecipes[screen.getChosenRecipe()]->mIngredients.size();
        else
            mChosenIngredient--;
        screen.setChosenIngredient(mChosenIngredient);
        screen.setChosenMaterial(0);
        break;
    case SDLK_L:
    case SDLK_RETURN: {
        auto &currentRecipe = screen.getCurrentRecipe();
        if ((size_t)mChosenIngredient == currentRecipe->mIngredients.size()) {
            if (screen.currentRecipeSatisfied()) {
                if (rm.mRecipes[screen.getChosenRecipe()]->mGoesIntoInventory || screen.isHaveChosenPositionInWorld()) {
                    screen.buildItem(Point{0, 0});
                    screen.reset();
                    return std::make_unique<ChoosingRecipeCraftingScreenState>();
                } else {
                    // Get player to build object into the world
                    //                        mChoosingPositionInWorld = true;
                    return std::make_unique<ChoosingBuildPositionCraftingScreenState>();
                }
            }
        } else if (currentRecipe->mIngredients[mChosenIngredient].mQuantity > 0 &&
                   !screen.filterInventoryForChosenMaterials().empty())
            return std::make_unique<ChoosingMaterialCraftingScreenState>();
        break;
    }
    case SDLK_H:
    case SDLK_BACKSPACE:
        screen.setChosenMaterial(0);
        //            mLayer = CraftingLayer::RECIPE;
        screen.getCurrentRecipe().release(); // NOLINT(bugprone-unused-return-value)
        screen.getCurrentlyChosenMaterials().clear();
        return std::make_unique<ChoosingRecipeCraftingScreenState>();
    }

    return nullptr;
}