#pragma once

#include "../../Recipe.h"
#include "../../UIState.h"
#include "Screen.h"

struct PlayerEntity;
struct CraftingScreen : Screen {
    explicit CraftingScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;

    /// Render crafting screen
    /// \param font Font object to render using
    void render(Font &font) override;
    void reset();

    void enable() override;

    enum class CraftingLayer { RECIPE, INGREDIENT, MATERIAL };

    int getChosenRecipe() const;
    void setChosenRecipe(int chosenRecipe);
    int getChosenIngredient() const;
    void setChosenIngredient(int chosenIngredient);
    void setChosenMaterial(int chosenMaterial);
    void setLayer(CraftingLayer layer);
    void setChoosingPositionInWorld(bool choosingPositionInWorld);
    bool isHaveChosenPositionInWorld() const;
    void setCouldNotBuildAtPosition(bool couldNotBuildAtPosition);

    PlayerEntity &getPlayer() const;
    std::vector<std::string> &getCurrentlyChosenMaterials();

    void setCurrentRecipe(std::unique_ptr<Recipe> currentRecipe);
    std::unique_ptr<Recipe> &getCurrentRecipe();

    /// Filter inventory items for items that are of the currently chosen material type and are not in
    /// currentlyChosenMaterials
    /// \return vector of shared pointers to the inventory items as described above
    std::vector<Entity *> filterInventoryForChosenMaterials();
    bool currentRecipeSatisfied();
    void buildItem(Point pos);

  private:
    int mChosenRecipe{0};
    int mChosenIngredient{0};
    int mChosenMaterial{0};
    PlayerEntity &mPlayer;
    CraftingLayer mLayer{CraftingLayer::RECIPE};
    std::unique_ptr<Recipe> mCurrentRecipe{nullptr};

    bool mChoosingPositionInWorld{false};
    bool mHaveChosenPositionInWorld{false};
    bool mCouldNotBuildAtPosition{false};

    std::vector<std::string> mCurrentlyChosenMaterials;

    std::unique_ptr<CraftingScreenState> mState{std::make_unique<ChoosingRecipeCraftingScreenState>()};
};
