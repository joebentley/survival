//
// Created by Joe Bentley on 25/05/2018.
//

#include "UIState.h"
#include "UI.h"

void ViewingInventoryState::onEntry(InventoryScreen &screen) {
    mChosenIndex = screen.getChosenIndex();
}

std::unique_ptr<InventoryScreenState>
ViewingInventoryState::handleInput(InventoryScreen &screen, SDL_KeyboardEvent &e) {
    auto &player = screen.getPlayer();

    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.disable();
            break;
        case SDLK_j:
            if (!player.isInventoryEmpty()) {
                if (mChosenIndex < (player.getInventorySize() - 1))
                    mChosenIndex++;
                else
                    mChosenIndex = 0;
            }
            screen.setChosenIndex(mChosenIndex);
            break;
        case SDLK_k:
            if (!player.isInventoryEmpty()) {
                if (mChosenIndex > 0)
                    mChosenIndex--;
                else
                    mChosenIndex = (int) player.getInventorySize() - 1;
            }
            screen.setChosenIndex(mChosenIndex);
            break;
        case SDLK_d:
            if (!player.isInventoryEmpty()) {
                auto itemID = player.getInventoryItemID(mChosenIndex);
                if (player.hasEquipped(itemID))
                    player.unequip(itemID);

                player.dropItem(mChosenIndex);

                if (player.getInventorySize() - 1 < mChosenIndex) {
                    mChosenIndex--;
                    screen.setChosenIndex(mChosenIndex);
                }
            }
            break;
        case SDLK_e:
            if (!player.isInventoryEmpty()) {
                auto item = player.getInventoryItem(mChosenIndex);
                if (item->hasBehaviour("EatableBehaviour")) {
                    player.addHunger(dynamic_cast<EatableBehaviour &>(*(*item).getBehaviourByID("EatableBehaviour")).hungerRestoration);
                    player.removeFromInventory(mChosenIndex);
                    item->destroy();
                    if (player.getInventorySize() - 1 < mChosenIndex) {
                        mChosenIndex--;
                        screen.setChosenIndex(mChosenIndex);
                    }
                }
            }
            break;
        case SDLK_a:
            if (!player.isInventoryEmpty()) {
                auto item = player.getInventoryItem(mChosenIndex);
                if (item->hasBehaviour("ApplyableBehaviour")) {
                    dynamic_cast<ApplyableBehaviour&>(*(*item).getBehaviourByID("ApplyableBehaviour")).apply();
                    if (player.getInventorySize() - 1 < mChosenIndex) {
                        mChosenIndex--;
                        screen.setChosenIndex(mChosenIndex);
                    }
                }
            }
            break;
        case SDLK_RETURN:
            if (!player.isInventoryEmpty())
                return std::make_unique<ViewingDescriptionInventoryState>();
            break;
    }

    return nullptr;
}

void ViewingInventoryState::onExit(InventoryScreen &screen) {
}

void ViewingDescriptionInventoryState::onEntry(InventoryScreen &screen) {
    screen.setViewingDescription(true);
}

std::unique_ptr<InventoryScreenState> ViewingDescriptionInventoryState::handleInput(InventoryScreen &screen,
                                                                                    SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            return std::make_unique<ViewingInventoryState>();
    }

    return nullptr;
}

void ViewingDescriptionInventoryState::onExit(InventoryScreen &screen) {
    screen.setViewingDescription(false);
}

void ViewingLootingDialogState::onEntry(LootingDialog &screen) {
    mChosenIndex = screen.getChosenIndex();
}

std::unique_ptr<LootingDialogState>
ViewingLootingDialogState::handleInput(LootingDialog &screen, SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.disable();
            break;
        case SDLK_RETURN:
            return std::make_unique<ViewingDescriptionLootingDialogState>();
        case SDLK_j:
            if (!screen.getItemsToShow().empty()) {
                if (mChosenIndex < (screen.getItemsToShow().size() - 1))
                    mChosenIndex++;
                else
                    mChosenIndex = 0;
                screen.setChosenIndex(mChosenIndex);
            }
            break;
        case SDLK_k:
            if (!screen.getItemsToShow().empty()) {
                if (mChosenIndex > 0)
                    mChosenIndex--;
                else
                    mChosenIndex = (int) screen.getItemsToShow().size() - 1;
                screen.setChosenIndex(mChosenIndex);
            }
            break;
        case SDLK_g:
            auto &itemsToShow = screen.getItemsToShow();
            auto entityToTransferFrom = screen.getEntityToTransferFrom();
            if (screen.getPlayer().addToInventory(itemsToShow[mChosenIndex]->mID)) {
                if (entityToTransferFrom != nullptr) {
                    entityToTransferFrom->dropItem(mChosenIndex);
                }

                itemsToShow.erase(itemsToShow.begin() + mChosenIndex);
                mChosenIndex = 0;
                screen.setChosenIndex(mChosenIndex);
            } else {
                return std::make_unique<ShowingTooMuchWeightMessageLootingDialogState>();
            }
            break;
    }

    return nullptr;
}

void ViewingLootingDialogState::onExit(LootingDialog &screen) {
}

void ViewingDescriptionLootingDialogState::onEntry(LootingDialog &screen) {
    screen.setViewingDescription(true);
}

std::unique_ptr<LootingDialogState>
ViewingDescriptionLootingDialogState::handleInput(LootingDialog &screen, SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            return std::make_unique<ViewingLootingDialogState>();
    }

    return nullptr;
}

void ViewingDescriptionLootingDialogState::onExit(LootingDialog &screen) {
    screen.setViewingDescription(false);
}

void ShowingTooMuchWeightMessageLootingDialogState::onEntry(LootingDialog &screen) {
    screen.setShowingTooMuchWeightMessage(true);
}

std::unique_ptr<LootingDialogState>
ShowingTooMuchWeightMessageLootingDialogState::handleInput(LootingDialog &screen, SDL_KeyboardEvent &e)
{
    switch (e.keysym.sym) {
        case SDLK_RETURN:
        case SDLK_ESCAPE:
            return std::make_unique<ViewingLootingDialogState>();
    }

    return nullptr;
}

void ShowingTooMuchWeightMessageLootingDialogState::onExit(LootingDialog &screen) {
    screen.setShowingTooMuchWeightMessage(false);
}

void ChoosingRecipeCraftingScreenState::onEntry(CraftingScreen &screen) {
    screen.setLayer(CraftingScreen::CraftingLayer::RECIPE);
    mChosenRecipe = screen.getChosenRecipe();
}

std::unique_ptr<CraftingScreenState>
ChoosingRecipeCraftingScreenState::handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e)
{
    auto &rm = RecipeManager::getInstance();

    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.disable();
            return nullptr;
        case SDLK_j:
            if (mChosenRecipe == rm.mRecipes.size() - 1)
                mChosenRecipe = 0;
            else
                mChosenRecipe++;

            screen.setChosenIngredient(0);
            screen.setChosenMaterial(0);
            screen.setChosenRecipe(mChosenRecipe);
            break;
        case SDLK_k:
            if (mChosenRecipe == 0)
                mChosenRecipe = (int) rm.mRecipes.size() - 1;
            else
                mChosenRecipe--;

            screen.setChosenIngredient(0);
            screen.setChosenMaterial(0);
            screen.setChosenRecipe(mChosenRecipe);
            break;
        case SDLK_l:
        case SDLK_RETURN:
            screen.setCurrentRecipe(std::make_unique<Recipe>(Recipe(*rm.mRecipes[mChosenRecipe])));
            return std::make_unique<ChoosingIngredientCraftingScreenState>();
    }
    return nullptr;
}

void ChoosingIngredientCraftingScreenState::onEntry(CraftingScreen &screen) {
    mChosenIngredient = screen.getChosenIngredient();
    screen.setLayer(CraftingScreen::CraftingLayer::INGREDIENT);
}

std::unique_ptr<CraftingScreenState>
ChoosingIngredientCraftingScreenState::handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e)
{
    auto &rm = RecipeManager::getInstance();
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.reset();
            screen.disable();
            break;
        case SDLK_j:
            if (mChosenIngredient == rm.mRecipes[screen.getChosenRecipe()]->mIngredients.size())
                mChosenIngredient = 0;
            else
                mChosenIngredient++;
            screen.setChosenIngredient(mChosenIngredient);
            screen.setChosenMaterial(0);
            break;
        case SDLK_k:
            if (mChosenIngredient == 0)
                mChosenIngredient = (int) rm.mRecipes[screen.getChosenRecipe()]->mIngredients.size();
            else
                mChosenIngredient--;
            screen.setChosenIngredient(mChosenIngredient);
            screen.setChosenMaterial(0);
            break;
        case SDLK_l:
        case SDLK_RETURN: {
            auto &currentRecipe = screen.getCurrentRecipe();
            if (mChosenIngredient == currentRecipe->mIngredients.size()) {
                if (screen.currentRecipeSatisfied()) {
                    if (rm.mRecipes[screen.getChosenRecipe()]->mGoesIntoInventory ||
                        screen.isHaveChosenPositionInWorld()) {
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
        case SDLK_h:
        case SDLK_BACKSPACE:
            screen.setChosenMaterial(0);
//            mLayer = CraftingLayer::RECIPE;
            screen.getCurrentRecipe().release();
            screen.getCurrentlyChosenMaterials().clear();
            return std::make_unique<ChoosingRecipeCraftingScreenState>();
    }

    return nullptr;
}

void ChoosingMaterialCraftingScreenState::onEntry(CraftingScreen &screen) {
    screen.setLayer(CraftingScreen::CraftingLayer::MATERIAL);
}

std::unique_ptr<CraftingScreenState>
ChoosingMaterialCraftingScreenState::handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.reset();
            screen.disable();
            break;
        case SDLK_j: {
            auto inventoryMaterials = screen.filterInventoryForChosenMaterials();
            if (mChosenMaterial == inventoryMaterials.size() - 1)
                mChosenMaterial = 0;
            else
                mChosenMaterial++;
            screen.setChosenMaterial(mChosenMaterial);
            break;
        }
        case SDLK_k: {
            auto inventoryMaterials = screen.filterInventoryForChosenMaterials();
            if (mChosenMaterial == 0)
                mChosenMaterial = (int) inventoryMaterials.size() - 1;
            else
                mChosenMaterial--;
            screen.setChosenMaterial(mChosenMaterial);
            break;
        }
        case SDLK_l:
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
                // This means we have now ran out of the desired material and should ensure that we leave the current crafting layer
            else if (inventoryMaterials.size() == 1) {
                return std::make_unique<ChoosingIngredientCraftingScreenState>();
            }
            break;
        }
        case SDLK_h:
        case SDLK_BACKSPACE:
            screen.setChosenMaterial(0);
            return std::make_unique<ChoosingIngredientCraftingScreenState>();
    }
    return nullptr;
}

void ChoosingBuildPositionCraftingScreenState::tryToBuildAtPosition(CraftingScreen &screen, Point posOffset) {
    auto p = posOffset + screen.getPlayer().getPos();
    if (EntityManager::getInstance().getEntitiesAtPosFaster(p).empty()) {
        mHaveChosenPositionInWorld = true;
        screen.buildItem(p);
    } else {
        screen.setCouldNotBuildAtPosition(true);
    }
}

void ChoosingBuildPositionCraftingScreenState::onEntry(CraftingScreen &screen) {
    screen.setChoosingPositionInWorld(true);
}

std::unique_ptr<CraftingScreenState>
ChoosingBuildPositionCraftingScreenState::handleInput(CraftingScreen &screen, SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_j:
            tryToBuildAtPosition(screen, Point {0, 1});
            break;
        case SDLK_k:
            tryToBuildAtPosition(screen, Point {0, -1});
            break;
        case SDLK_l:
            tryToBuildAtPosition(screen, Point {1, 0});
            break;
        case SDLK_h:
            tryToBuildAtPosition(screen, Point {-1, 0});
            break;
        case SDLK_y:
            tryToBuildAtPosition(screen, Point {-1, -1});
            break;
        case SDLK_u:
            tryToBuildAtPosition(screen, Point {1, -1});
            break;
        case SDLK_b:
            tryToBuildAtPosition(screen, Point {-1, 1});
            break;
        case SDLK_n:
            tryToBuildAtPosition(screen, Point {1, 1});
            break;
    }

    if (mHaveChosenPositionInWorld)
        return std::make_unique<ChoosingRecipeCraftingScreenState>();

    return nullptr;
}

void ChoosingBuildPositionCraftingScreenState::onExit(CraftingScreen &screen) {
    screen.setChoosingPositionInWorld(true);
}

void ChoosingSlotEquipmentScreenState::onEntry(EquipmentScreen &screen) {
    mChosenSlot = screen.getChosenSlot();
}

std::unique_ptr<EquipmentScreenState>
ChoosingSlotEquipmentScreenState::handleInput(EquipmentScreen &screen, SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            screen.reset();
            screen.disable();
            break;
        case SDLK_j:
            ++mChosenSlot;
            screen.setChosenSlot(mChosenSlot);
            break;
        case SDLK_k:
            --mChosenSlot;
            screen.setChosenSlot(mChosenSlot);
            break;
        case SDLK_RETURN:
            auto &player = screen.getPlayer();
            if (!player.hasEquippedInSlot(mChosenSlot) && !player.getInventoryItemsEquippableInSlot(mChosenSlot).empty())
                return std::make_unique<ChoosingNewEquipmentScreenState>();
            else
                return std::make_unique<ChoosingActionEquipmentScreenState>();
    }

    return nullptr;
}

void ChoosingSlotEquipmentScreenState::onExit(EquipmentScreen &screen) {}

void ChoosingNewEquipmentScreenState::onEntry(EquipmentScreen &screen) {
    screen.setChoosingNewEquipment(true);
}

std::unique_ptr<EquipmentScreenState>
ChoosingNewEquipmentScreenState::handleInput(EquipmentScreen &screen, SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            return std::make_unique<ChoosingSlotEquipmentScreenState>();
        case SDLK_j: {
            auto equippableIDs = screen.getPlayer().getInventoryItemsEquippableInSlot(screen.getChosenSlot());
            if (mChoosingNewEquipmentIndex == equippableIDs.size() - 1)
                mChoosingNewEquipmentIndex = 0;
            else
                ++mChoosingNewEquipmentIndex;
            screen.setChoosingNewEquipmentIndex(mChoosingNewEquipmentIndex);
            break;
        }
        case SDLK_k: {
            auto equippableIDs = screen.getPlayer().getInventoryItemsEquippableInSlot(screen.getChosenSlot());
            if (mChoosingNewEquipmentIndex == 0)
                mChoosingNewEquipmentIndex = static_cast<int>(equippableIDs.size()) - 1;
            else
                --mChoosingNewEquipmentIndex;
            screen.setChoosingNewEquipmentIndex(mChoosingNewEquipmentIndex);
            break;
        }
        case SDLK_RETURN: {
            auto &player = screen.getPlayer();
            auto chosenSlot = screen.getChosenSlot();
            auto equippableIDs = player.getInventoryItemsEquippableInSlot(chosenSlot);
            player.equip(chosenSlot, equippableIDs[mChoosingNewEquipmentIndex]);
            screen.reset();
            return std::make_unique<ChoosingSlotEquipmentScreenState>();
        }
    }

    return nullptr;
}

void ChoosingNewEquipmentScreenState::onExit(EquipmentScreen &screen) {
    screen.setChoosingNewEquipment(false);
}

void ChoosingActionEquipmentScreenState::onEntry(EquipmentScreen &screen) {
    screen.setChoosingEquipmentAction(true);
}

std::unique_ptr<EquipmentScreenState>
ChoosingActionEquipmentScreenState::handleInput(EquipmentScreen &screen, SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            return std::make_unique<ChoosingSlotEquipmentScreenState>();
        case SDLK_RETURN: {
            auto &player = screen.getPlayer();
            auto chosenSlot = screen.getChosenSlot();
            if (player.hasEquippedInSlot(chosenSlot)) {
                player.unequip(chosenSlot);
            } else if (!player.getInventoryItemsEquippableInSlot(chosenSlot).empty()) {
                return std::make_unique<ChoosingNewEquipmentScreenState>();
            }
            return std::make_unique<ChoosingSlotEquipmentScreenState>();
        }
    }

    return nullptr;
}

void ChoosingActionEquipmentScreenState::onExit(EquipmentScreen &screen) {
    screen.setChoosingEquipmentAction(false);
    screen.setChoosingEquipmentActionIndex(0);
}
