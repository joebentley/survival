#include "CraftingScreen.h"

#include "../../Entity/EntityManager.h"
#include "../../Entity/PlayerEntity.h"
#include "../../Font.h"
#include "../../Properties.h"
#include "../NotificationMessageRenderer.h"

void CraftingScreen::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }

    mShouldRenderWorld = mChoosingPositionInWorld;
}

void CraftingScreen::render(Font &font) {
    if (mChoosingPositionInWorld) {
        auto player = EntityManager::getInstance().getEntityByID("Player");

        int y = 0;
        if (World::worldToScreen(player->getPos()).mY < World::SCREEN_HEIGHT / 2)
            y = World::SCREEN_HEIGHT - 1;

        std::string message = "Choose direction to place object";
        if (mCouldNotBuildAtPosition)
            message = "Please choose a square that does not already have an entity on";

        font.drawText(message, 1, y, Color::getColor("white"), Color::getColor("black"));
        return;
    }

    auto &rm = RecipeManager::getInstance();
    const int xOffset = 3;
    const int yOffset = 3;

    for (std::vector<std::shared_ptr<Recipe>>::size_type i = 0; i < rm.mRecipes.size(); ++i) {
        auto recipe = *rm.mRecipes.at(i);
        Color bColor = Color::getColor("black");

        if (i == (size_t)mChosenRecipe)
            bColor = Color::getColor("blue");

        font.drawText(recipe.mNameOfProduct, xOffset, yOffset + (int)i, Color::getColor("white"), bColor);
    }

    auto &ingredients = rm.mRecipes[mChosenRecipe]->mIngredients;
    for (std::vector<Recipe::Ingredient>::size_type i = 0; i < ingredients.size() + 1; ++i) {
        Color bColor = Color::getColor("black");

        if ((mLayer == CraftingLayer::INGREDIENT || mLayer == CraftingLayer::MATERIAL) &&
            i == (size_t)mChosenIngredient)
            bColor = Color::getColor("blue");

        if (i != ingredients.size()) {
            auto ingredient = ingredients.at(i);
            if (mCurrentRecipe != nullptr)
                ingredient = mCurrentRecipe->mIngredients[i];

            font.drawText(std::to_string(ingredient.mQuantity) + "x " + ingredient.mEntityType, xOffset + 14,
                          yOffset + (int)i, Color::getColor("white"), bColor);
        } else {
            Color fColor = Color::getColor("grey");

            if (currentRecipeSatisfied())
                fColor = Color::getColor("white");

            font.drawText("Construct", xOffset + 14, yOffset + (int)i, fColor, bColor);
        }
    }

    if (mLayer == CraftingLayer::INGREDIENT || mLayer == CraftingLayer::MATERIAL) {
        std::vector<Entity *> inventoryMaterials = filterInventoryForChosenMaterials();

        for (std::vector<Entity *>::size_type i = 0; i < inventoryMaterials.size(); ++i) {
            Color bColor;
            auto &material = inventoryMaterials.at(i);

            if (mLayer == CraftingLayer::MATERIAL && i == (size_t)mChosenMaterial) {
                bColor = Color::getColor("blue");
                font.drawText(material->mGraphic + " " + material->mName, xOffset + 24, yOffset + (int)i, bColor);
            } else
                font.drawText(material->mGraphic + " " + material->mName, xOffset + 24, yOffset + (int)i);
        }
    }
}

std::vector<Entity *> CraftingScreen::filterInventoryForChosenMaterials() {
    auto &rm = RecipeManager::getInstance();

    std::vector<Entity *> inventoryMaterials;
    auto inventory = mPlayer.getInventory();
    std::copy_if(inventory.cbegin(), inventory.cend(), std::back_inserter(inventoryMaterials), [this, &rm](auto &a) {
        if (!a->hasProperty("CraftingMaterial"))
            return false;
        if (std::find(mCurrentlyChosenMaterials.begin(), mCurrentlyChosenMaterials.end(), a->mID) !=
            mCurrentlyChosenMaterials.end())
            return false;

        auto type = std::any_cast<CraftingMaterialProperty::Data>(a->getProperty("CraftingMaterial")->getValue()).type;
        return rm.mRecipes[mChosenRecipe]->mIngredients[mChosenIngredient].mEntityType == type;
    });

    return inventoryMaterials;
}

bool CraftingScreen::currentRecipeSatisfied() {
    return (mCurrentRecipe != nullptr &&
            std::all_of(mCurrentRecipe->mIngredients.begin(), mCurrentRecipe->mIngredients.end(),
                        [](auto &a) { return a.mQuantity == 0; }));
}

void CraftingScreen::reset() {
    mCurrentlyChosenMaterials.clear();
    mCurrentRecipe = nullptr;
    mChosenRecipe = 0;
    mChosenIngredient = 0;
    mChosenMaterial = 0;
    mLayer = CraftingLayer::RECIPE;
    mChoosingPositionInWorld = false;
    mHaveChosenPositionInWorld = false;
    mCouldNotBuildAtPosition = false;
    mShouldRenderWorld = false;
    mState = std::make_unique<ChoosingRecipeCraftingScreenState>();
}

void CraftingScreen::buildItem(Point pos) {
    auto &recipe = RecipeManager::getInstance().mRecipes[mChosenRecipe];

    if (!recipe->mGoesIntoInventory) {
        recipe->mPointIfNotGoingIntoInventory = pos;
        mEnabled = false;
    }

    recipe->produce();

    for (const auto &ID : mCurrentlyChosenMaterials) {
        mPlayer.removeFromInventory(ID);
        EntityManager::getInstance().eraseByID(ID);
    }

    NotificationMessageRenderer::getInstance().queueMessage("Created " + recipe->mNameOfProduct);
}

void CraftingScreen::enable() {
    Screen::enable();
    reset();
}

int CraftingScreen::getChosenRecipe() const { return mChosenRecipe; }

void CraftingScreen::setChosenRecipe(int chosenRecipe) { mChosenRecipe = chosenRecipe; }

int CraftingScreen::getChosenIngredient() const { return mChosenIngredient; }

void CraftingScreen::setChosenIngredient(int chosenIngredient) { mChosenIngredient = chosenIngredient; }

void CraftingScreen::setChosenMaterial(int chosenMaterial) { mChosenMaterial = chosenMaterial; }

void CraftingScreen::setLayer(CraftingScreen::CraftingLayer layer) { mLayer = layer; }

void CraftingScreen::setChoosingPositionInWorld(bool choosingPositionInWorld) {
    mChoosingPositionInWorld = choosingPositionInWorld;
}

bool CraftingScreen::isHaveChosenPositionInWorld() const { return mHaveChosenPositionInWorld; }

void CraftingScreen::setCouldNotBuildAtPosition(bool couldNotBuildAtPosition) {
    mCouldNotBuildAtPosition = couldNotBuildAtPosition;
}

PlayerEntity &CraftingScreen::getPlayer() const { return mPlayer; }

std::vector<std::string> &CraftingScreen::getCurrentlyChosenMaterials() { return mCurrentlyChosenMaterials; }

void CraftingScreen::setCurrentRecipe(std::unique_ptr<Recipe> currentRecipe) {
    mCurrentRecipe = std::move(currentRecipe);
}

std::unique_ptr<Recipe> &CraftingScreen::getCurrentRecipe() { return mCurrentRecipe; }
