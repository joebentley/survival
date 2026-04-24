#include "ChoosingMaterialCraftingScreenState.h"

#include "../../../Entity/Entity.h"
#include "../../Screens/CraftingScreen.h"
#include "ChoosingIngredientCraftingScreenState.h"

void ChoosingMaterialCraftingScreenState::onEntry(CraftingScreen &screen) {
    screen.setLayer(CraftingScreen::CraftingLayer::MATERIAL);
}

std::unique_ptr<CraftingScreenState> ChoosingMaterialCraftingScreenState::handleInput(CraftingScreen &screen,
                                                                                      SDL_KeyboardEvent &e) {
    switch (e.key) {
    case SDLK_ESCAPE:
        screen.reset();
        screen.disable();
        break;
    case SDLK_J: {
        auto inventoryMaterials = screen.filterInventoryForChosenMaterials();
        if ((size_t)mChosenMaterial == inventoryMaterials.size() - 1)
            mChosenMaterial = 0;
        else
            mChosenMaterial++;
        screen.setChosenMaterial(mChosenMaterial);
        break;
    }
    case SDLK_K: {
        auto inventoryMaterials = screen.filterInventoryForChosenMaterials();
        if (mChosenMaterial == 0)
            mChosenMaterial = (int)inventoryMaterials.size() - 1;
        else
            mChosenMaterial--;
        screen.setChosenMaterial(mChosenMaterial);
        break;
    }
    case SDLK_L:
    case SDLK_RETURN: {
        auto inventoryMaterials = screen.filterInventoryForChosenMaterials();
        auto &currentRecipe = screen.getCurrentRecipe();
        auto chosenIngredient = screen.getChosenIngredient();

        if (currentRecipe->mIngredients[chosenIngredient].mQuantity > 0) {
            screen.getCurrentlyChosenMaterials().emplace_back(inventoryMaterials[mChosenMaterial]->mID);
            currentRecipe->mIngredients[chosenIngredient].mQuantity--;
        }
        // Have finished this material requirement
        if (currentRecipe->mIngredients[chosenIngredient].mQuantity == 0) {
            chosenIngredient++; // Automatically go to next ingredient for faster crafting
            screen.setChosenIngredient(chosenIngredient);
            return std::make_unique<ChoosingIngredientCraftingScreenState>();
        }
        // This means we have now ran out of the desired material and should ensure that we leave the current crafting
        // layer
        else if (inventoryMaterials.size() == 1) {
            return std::make_unique<ChoosingIngredientCraftingScreenState>();
        }
        break;
    }
    case SDLK_H:
    case SDLK_BACKSPACE:
        screen.setChosenMaterial(0);
        return std::make_unique<ChoosingIngredientCraftingScreenState>();
    }
    return nullptr;
}
