#include "dialog.h"
#include "utils.h"
#include "behaviours.h"
#include "world.h"

#include <sstream>
#include <algorithm>

std::string repeat(int n, const std::string &str) {
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << str;
    return os.str();
}

void showMessageBox(Font &font, const std::vector<std::string> &contents, int padding, int x, int y) {
    int maxNumChars = 0;

    for (const auto &string : contents) {
        int length = getFontStringLength(string);
        if (length > maxNumChars)
            maxNumChars = length;
    }

    int innerBoxWidth = maxNumChars + 2 * padding;

    font.drawText("${black}$(p23)" + repeat(innerBoxWidth, "$(p27)") + "$(p9)", x, y);

    for (int i = 0; i < padding; ++i) {
        font.drawText("${black}$(p8)" + std::string((unsigned long)innerBoxWidth, ' ') + "$(p8)", x, ++y);
    }

    for (const auto &line : contents) {
        int paddingLength = maxNumChars - getFontStringLength(line);
        font.drawText("${black}$(p8)" + std::string((unsigned long)padding, ' ') + line +
                      std::string((unsigned long)paddingLength, ' ') +
                      "${black}$[white]" + std::string((unsigned long)padding, ' ') + "$(p8)", x, ++y);
    }

    for (int i = 0; i < padding; ++i) {
        font.drawText("${black}$(p8)" + std::string((unsigned long)innerBoxWidth, ' ') + "$(p8)", x, ++y);
    }

    font.drawText("${black}$(p22)" + repeat(innerBoxWidth, "$(p27)") + "$(p10)", x, y+1);
}

void drawDescriptionScreen(Font& font, Entity& item) {
    font.drawText(item.graphic + " " + item.name, InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET);
    font.drawText(item.shortDesc, InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 2);

    auto words = wordWrap(item.longDesc, InventoryScreen::WORD_WRAP_COLUMN);
    for (int i = 0; i < words.size(); ++i) {
        font.drawText(words[i], InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 4 + i);
    }

    auto b = item.getBehaviourByID("PickuppableBehaviour");

    if (b != nullptr) {
        int weight = dynamic_cast<PickuppableBehaviour &>(*b).weight;
        font.drawText("It weighs " + std::to_string(weight) + (weight == 1 ? " pound" : " pounds"),
                      InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 4 + words.size() + 2);
    }

    font.drawText("esc-back", 1, SCREEN_HEIGHT - 2);
}

void InventoryScreen::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            if (viewingDescription)
                viewingDescription = false;
            else
                enabled = false;
            break;
        case SDLK_j:
            if (!viewingDescription && !player.isInventoryEmpty()) {
                if (chosenIndex < (player.getInventorySize() - 1))
                    chosenIndex++;
                else
                    chosenIndex = 0;
            }
            break;
        case SDLK_k:
            if (!viewingDescription && !player.isInventoryEmpty()) {
                if (chosenIndex > 0)
                    chosenIndex--;
                else
                    chosenIndex = (int) player.getInventorySize() - 1;
            }
            break;
        case SDLK_d:
            if (!viewingDescription && !player.isInventoryEmpty()) {
                auto itemID = player.getInventoryItemID(chosenIndex);
                if (player.hasEquipped(itemID))
                    player.unequip(itemID);

                player.dropItem(chosenIndex);

                if (player.getInventorySize() - 1 < chosenIndex)
                    chosenIndex--;
            }
            break;
        case SDLK_e:
            if (!viewingDescription && !player.isInventoryEmpty()) {
                auto item = player.getInventoryItem(chosenIndex);
                if (item->hasBehaviour("EatableBehaviour")) {
                    player.addHunger(dynamic_cast<EatableBehaviour &>(*(*item).getBehaviourByID("EatableBehaviour")).hungerRestoration);
                    player.removeFromInventory(chosenIndex);
                    item->destroy();
                    if (player.getInventorySize() - 1 < chosenIndex)
                        chosenIndex--;
                }
            }
            break;
        case SDLK_a:
            if (!viewingDescription && !player.isInventoryEmpty()) {
                auto item = player.getInventoryItem(chosenIndex);
                if (item->hasBehaviour("ApplyableBehaviour")) {
                    dynamic_cast<ApplyableBehaviour&>(*(*item).getBehaviourByID("ApplyableBehaviour")).apply();
                    if (player.getInventorySize() - 1 < chosenIndex)
                        chosenIndex--;
                }
            }
            break;
        case SDLK_RETURN:
            if (!player.isInventoryEmpty())
                viewingDescription = true;
            break;
    }
}

void InventoryScreen::render(Font &font) {
    if (viewingDescription) {
        drawDescriptionScreen(font, *player.getInventoryItem(chosenIndex));
        return;
    }

    font.draw("right", X_OFFSET - 1, chosenIndex + Y_OFFSET);

    for (int i = 0; i < player.getInventorySize(); ++i) {
        auto item = player.getInventoryItem(i);
        std::string displayString = item->graphic + " " + item->name;

        font.drawText(displayString, X_OFFSET, i + Y_OFFSET);
        if (player.hasEquipped(item->ID))
            font.drawText("(" + slotToString(player.getEquipmentSlotByID(item->ID)) + ")",
                    X_OFFSET + getFontStringLength(displayString) + 2, i + Y_OFFSET);
    }

    std::string colorStr;
    float hpPercent = player.hp / player.maxhp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(player.hp))
                  + "/" + std::to_string((int)ceil(player.maxhp)), SCREEN_WIDTH - X_STATUS_OFFSET, 1);

    if (player.hunger > 0.7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - X_STATUS_OFFSET, 2);
    else if (player.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", SCREEN_WIDTH - X_STATUS_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", SCREEN_WIDTH - X_STATUS_OFFSET, 2);

    font.drawText("${black}" + std::to_string(player.getCarryingWeight()) + "/" + std::to_string(player.maxCarryWeight) + "lb",
                  SCREEN_WIDTH - X_STATUS_OFFSET, 4);

    std::string helpString;
    auto item = player.getInventoryItem(chosenIndex);
    if (item->hasBehaviour("EatableBehaviour"))
        helpString += "e-eat  ";
    if (item->hasBehaviour("ApplyableBehaviour"))
        helpString += "a-apply  ";
    font.drawText(helpString + "d-drop  return-view desc  esc-exit inv", 1, SCREEN_HEIGHT - 2);
}

void LootingDialog::showItemsToLoot(std::vector<std::shared_ptr<Entity>> items) {
    itemsToShow = std::move(items);
    enabled = true;
}


void LootingDialog::showItemsToLoot(std::vector<std::shared_ptr<Entity>> items, std::shared_ptr<Entity> entityToTransferFrom)
{
    itemsToShow = std::move(items);
    this->entityToTransferFrom = std::move(entityToTransferFrom);
    enabled = true;
}

void LootingDialog::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            if (viewingDescription)
                viewingDescription = false;
            else if (showingTooMuchWeightMessage)
                showingTooMuchWeightMessage = false;
            else {
                enabled = false;
                entityToTransferFrom = nullptr;
            }
            break;
        case SDLK_j:
            if (!showingTooMuchWeightMessage && !itemsToShow.empty()) {
                if (chosenIndex < (itemsToShow.size() - 1))
                    chosenIndex++;
                else
                    chosenIndex = 0;
            }
            break;
        case SDLK_k:
            if (!showingTooMuchWeightMessage && !itemsToShow.empty()) {
                if (chosenIndex > 0)
                    chosenIndex--;
                else
                    chosenIndex = (int) itemsToShow.size() - 1;
            }
            break;
        case SDLK_g:
            if (player.addToInventory(itemsToShow[chosenIndex])) {
                if (entityToTransferFrom != nullptr) {
                    entityToTransferFrom->dropItem(chosenIndex);
                }

                itemsToShow.erase(itemsToShow.begin() + chosenIndex);
                chosenIndex = 0;
            } else {
                showingTooMuchWeightMessage = true;
            }
            break;
        case SDLK_RETURN:
            if (!showingTooMuchWeightMessage)
                viewingDescription = true;
            else
                showingTooMuchWeightMessage = false;
            break;
    }
}

void LootingDialog::render(Font &font) {
    if (viewingDescription) {
        drawDescriptionScreen(font, *itemsToShow[chosenIndex]);
        return;
    }

    auto numItems = (int)itemsToShow.size();

    if (numItems == 0) {
        enabled = false;
        entityToTransferFrom = nullptr;
        return;
    }

    const int x = SCREEN_WIDTH / 2 - (DIALOG_WIDTH + 4) / 2;
    const int y = 10;

    font.drawText("${black}$(p23)" + repeat(DIALOG_WIDTH + 4, "$(p27)") + "$(p9)", x, y);
    font.drawText("${black}$(p8)" + std::string(DIALOG_WIDTH + 4, ' ') + "$(p8)", x, y+1);

    for (int i = 0; i < numItems; ++i) {
        auto item = itemsToShow[i];
        int weight = dynamic_cast<PickuppableBehaviour&>(*item->getBehaviourByID("PickuppableBehaviour")).weight;

        std::string weightString = std::to_string(weight);
        std::string string = item->graphic + " " + item->name.substr(0, DIALOG_WIDTH - 6);
        string += std::string(DIALOG_WIDTH - getFontStringLength(string) - 3 - weightString.size() + 1, ' ') + "$[white]" + weightString + " lb";
        font.drawText("${black}$(p8)  " + string + "${black} $[white]$(p8)", x, y+2+i);
    }

    font.draw("right", x + 2, y + 2 + chosenIndex);

    font.drawText("${black}$(p8)" + std::string(DIALOG_WIDTH + 4, ' ') + "$(p8)", x, y+numItems+2);
    std::string string = "g-loot  esc-quit  return-desc";
    font.drawText("${black}$(p8)  " + string + std::string(DIALOG_WIDTH - string.size() + 2, ' ') + "$(p8)", x, y+numItems+3);
    font.drawText("${black}$(p22)" + repeat(DIALOG_WIDTH + 4, "$(p27)") + "$(p10)", x, y+numItems+4);


    if (showingTooMuchWeightMessage) {
        const std::string& displayString = "You cannot carry that much!";
        showMessageBox(font, displayString, 20, 20);
    }
}

inline Point InspectionDialog::clipToScreenEdge(const Point &p) const {
    auto worldPos = player.getWorldPos();
    auto point = p - Point(SCREEN_WIDTH, SCREEN_HEIGHT) * worldPos;
    Point returnPoint(p);
    if (point.x < 0)
        returnPoint.x = worldPos.x * SCREEN_WIDTH;
    else if (point.x > SCREEN_WIDTH - 1)
        returnPoint.x = (worldPos.x + 1) * SCREEN_WIDTH - 1;
    if (point.y < 0)
        returnPoint.y = worldPos.y * SCREEN_HEIGHT;
    else if (point.y > SCREEN_HEIGHT - 1)
        returnPoint.y = (worldPos.y + 1) * SCREEN_HEIGHT - 1;
    return returnPoint;
}

void InspectionDialog::handleInput(SDL_KeyboardEvent &e) {
    Point posOffset;

    switch (e.keysym.sym) {
        case SDLK_h:
            if (!viewingDescription)
                posOffset = Point(-1, 0);
            break;
        case SDLK_j:
            if (!viewingDescription)
                posOffset = Point(0, 1);
            break;
        case SDLK_k:
            if (!viewingDescription)
                posOffset = Point(0, -1);
            break;
        case SDLK_l:
            if (!viewingDescription)
                posOffset = Point(1, 0);
            break;
        case SDLK_y:
            if (!viewingDescription)
                posOffset = Point(-1, -1);
            break;
        case SDLK_u:
            if (!viewingDescription)
                posOffset = Point(1, -1);
            break;
        case SDLK_b:
            if (!viewingDescription)
                posOffset = Point(-1, 1);
            break;
        case SDLK_n:
            if (!viewingDescription)
                posOffset = Point(1, 1);
            break;
        case SDLK_EQUALS:
            if (selectingFromMultipleOptions && !viewingDescription) {
                const auto &currentEntities = EntityManager::getInstance().getEntitiesAtPos(chosenPoint);
                if (chosenIndex == currentEntities.size() - 1)
                    chosenIndex = 0;
                else
                    chosenIndex++;
            }
            break;
        case SDLK_MINUS:
            if (selectingFromMultipleOptions && !viewingDescription) {
                const auto &currentEntities = EntityManager::getInstance().getEntitiesAtPos(chosenPoint);
                if (chosenIndex == 0)
                    chosenIndex = (int)(currentEntities.size() - 1);
                else
                    chosenIndex--;
            }
            break;
        case SDLK_RETURN:
            if (thereIsAnEntity)
                viewingDescription = true;
            break;
        case SDLK_ESCAPE:
            if (viewingDescription)
                viewingDescription = false;
            else
                enabled = false;
            return;
        default:
            return;
    }

    if (e.keysym.mod & KMOD_SHIFT)
        posOffset *= 5;

    chosenPoint = clipToScreenEdge(chosenPoint + posOffset);
}

void InspectionDialog::render(Font &font) {
    const auto &entitiesAtPointBefore = EntityManager::getInstance().getEntitiesAtPos(chosenPoint);
    std::vector<std::shared_ptr<Entity>> entitiesAtPoint;

    std::copy_if(entitiesAtPointBefore.cbegin(), entitiesAtPointBefore.cend(), std::back_inserter(entitiesAtPoint),
    [] (auto &a) { return !a->isInAnInventory; });

    if (viewingDescription) {
        drawDescriptionScreen(font, *entitiesAtPoint[chosenIndex]);
        return;
    }

    const auto &chosenPointScreen = worldToScreen(chosenPoint);
    font.drawText("${black}$[yellow]X", chosenPointScreen);

    int xPosWindow = chosenPointScreen.x >= SCREEN_WIDTH / 2 ? 2 : SCREEN_WIDTH / 2 + 1;

    if (entitiesAtPoint.size() > 1) {
        selectingFromMultipleOptions = true;
        std::vector<std::string> lines;

        lines.emplace_back(" You see");
        std::transform(entitiesAtPoint.cbegin(), entitiesAtPoint.cend(), std::back_inserter(lines),
                       [] (auto &a) -> std::string { return " " + a->graphic + " " + a->name; });

        lines.emplace_back("");
        lines.emplace_back(" (-)-$(up) (=)-$(down) return-desc");
        showMessageBox(font, lines, xPosWindow - 1, 2);
        font.draw("right", xPosWindow + 2 - 1, 2 + 3 + chosenIndex);
        thereIsAnEntity = true;
    } else {
        selectingFromMultipleOptions = false;
        chosenIndex = 0;
    }

    if (entitiesAtPoint.size() == 1) {
        const auto &entity = *entitiesAtPoint[0];

        std::vector<std::string> lines;
        lines.emplace_back(entity.graphic + " " + entity.name);

        if (!entity.shortDesc.empty()) {
            lines.emplace_back("");
            auto descLines = wordWrap(entity.shortDesc, SCREEN_WIDTH / 2 - 5);
            std::copy(descLines.begin(), descLines.end(), std::back_inserter(lines));
        }

        if (!entity.longDesc.empty()) {
            lines.emplace_back("");
            auto descLines = wordWrap(entity.longDesc, SCREEN_WIDTH / 2 - 5);
            std::copy(descLines.begin(), descLines.end(), std::back_inserter(lines));
        }

        const int cappedNumLines = SCREEN_HEIGHT - 20;
        std::vector<std::string> linesCapped = lines;
        if (lines.size() > cappedNumLines) {
            linesCapped = std::vector<std::string>(lines.begin(), lines.begin() + cappedNumLines);
            *(linesCapped.end() - 1) += "...";
        }

        showMessageBox(font, lines, xPosWindow, 2);

        thereIsAnEntity = true;
    }

    if (entitiesAtPoint.empty())
        thereIsAnEntity = false;
}

void CraftingScreen::handleInput(SDL_KeyboardEvent &e) {
    auto &rm = RecipeManager::getInstance();
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            enabled = false;
            break;
        case SDLK_j:
            if (choosingPositionInWorld) {
                tryToBuildAtPosition(Point {0, 1});
                break;
            }

            if (layer == CraftingLayer::RECIPE) {
                if (chosenRecipe == rm.recipes.size() - 1)
                    chosenRecipe = 0;
                else
                    chosenRecipe++;
                chosenIngredient = 0;
                chosenMaterial = 0;
            } else if (layer == CraftingLayer::INGREDIENT) {
                if (chosenIngredient == rm.recipes[chosenRecipe]->ingredients.size())
                    chosenIngredient = 0;
                else
                    chosenIngredient++;
                chosenMaterial = 0;
            } else if (layer == CraftingLayer::MATERIAL) {
                std::vector<std::shared_ptr<Entity>> inventoryMaterials = filterInventoryForChosenMaterials();
                if (chosenMaterial == inventoryMaterials.size() - 1)
                    chosenMaterial = 0;
                else
                    chosenMaterial++;
            }
            break;
        case SDLK_k:
            if (choosingPositionInWorld) {
                tryToBuildAtPosition(Point {0, -1});
                break;
            }

            if (layer == CraftingLayer::RECIPE) {
                if (chosenRecipe == 0)
                    chosenRecipe = (int) rm.recipes.size() - 1;
                else
                    chosenRecipe--;
                chosenIngredient = 0;
                chosenMaterial = 0;
            } else if (layer == CraftingLayer::INGREDIENT) {
                if (chosenIngredient == 0)
                    chosenIngredient = (int)rm.recipes[chosenRecipe]->ingredients.size();
                else
                    chosenIngredient--;
                chosenMaterial = 0;
            } else if (layer == CraftingLayer::MATERIAL) {
                std::vector<std::shared_ptr<Entity>> inventoryMaterials = filterInventoryForChosenMaterials();
                if (chosenMaterial == 0)
                    chosenMaterial = (int)inventoryMaterials.size() - 1;
                else
                    chosenMaterial--;
            }
            break;
        case SDLK_l:
            if (choosingPositionInWorld) {
                tryToBuildAtPosition(Point {1, 0});
                break;
            }
        case SDLK_RETURN:
            if (layer == CraftingLayer::RECIPE) {
                chosenIngredient = 0;
                chosenMaterial = 0;
                layer = CraftingLayer::INGREDIENT;

                currentRecipe = std::make_unique<Recipe>(Recipe(*rm.recipes[chosenRecipe]));
            } else if (layer == CraftingLayer::INGREDIENT) {
                if (chosenIngredient == currentRecipe->ingredients.size()) {
                    if (currentRecipeSatisfied()) {
                        if (rm.recipes[chosenRecipe]->goesIntoInventory || haveChosenPositionInWorld) {
                            buildItem(Point {0, 0});
                            this->reset();
                        } else {
                            // Get player to build object into the world
                            choosingPositionInWorld = true;
                            break;
                        }
                    }
                } else if (currentRecipe->ingredients[chosenIngredient].quantity > 0)
                    layer = CraftingLayer::MATERIAL;
            } else if (layer == CraftingLayer::MATERIAL) {
                auto inventoryMaterials = filterInventoryForChosenMaterials();

                if (currentRecipe->ingredients[chosenIngredient].quantity > 0) {
                    currentlyChosenMaterials.emplace_back(inventoryMaterials[chosenMaterial]->ID);
                    currentRecipe->ingredients[chosenIngredient].quantity--;
                }
                if (currentRecipe->ingredients[chosenIngredient].quantity == 0) {
                    layer = CraftingLayer::INGREDIENT;
                    chosenIngredient++; // Automatically go to next ingredient for faster crafting
                }
            }
            break;
        case SDLK_h:
            if (choosingPositionInWorld) {
                tryToBuildAtPosition(Point {-1, 0});
                break;
            }
        case SDLK_BACKSPACE:
            if (layer == CraftingLayer::INGREDIENT) {
                chosenMaterial = 0;
                layer = CraftingLayer::RECIPE;
                currentRecipe.release();
            } else if (layer == CraftingLayer::MATERIAL) {
                chosenMaterial = 0;
                layer = CraftingLayer::INGREDIENT;
            }
            break;
        case SDLK_y:
            if (choosingPositionInWorld)
                tryToBuildAtPosition(Point {-1, -1});
            break;
        case SDLK_u:
            if (choosingPositionInWorld)
                tryToBuildAtPosition(Point {1, -1});
            break;
        case SDLK_b:
            if (choosingPositionInWorld)
                tryToBuildAtPosition(Point {-1, 1});
            break;
        case SDLK_n:
            if (choosingPositionInWorld)
                tryToBuildAtPosition(Point {1, 1});
            break;

    }
}

void CraftingScreen::tryToBuildAtPosition(Point posOffset) {
    auto p = posOffset + player.getPos();
    if (EntityManager::getInstance().getEntitiesAtPos(p).empty()) {
        haveChosenPositionInWorld = true;
        buildItem(p);
    } else {
        couldNotBuildAtPosition = true;
    }
}

void CraftingScreen::render(Font &font, World &world, LightMapTexture &lightMapTexture) {
    if (choosingPositionInWorld) {
        auto player = EntityManager::getInstance().getEntityByID("Player");
        world.render(font, player->getWorldPos());
        EntityManager::getInstance().render(font, lightMapTexture);

        int y = 0;
        if (worldToScreen(player->getPos()).y < SCREEN_HEIGHT / 2)
            y = SCREEN_HEIGHT - 1;

        std::string message = "Choose direction to place object";
        if (couldNotBuildAtPosition)
            message = "Please choose a square that does not already have an entity on";

        font.drawText(message, 1, y, getColor("white"), getColor("black"));
        return;
    }

    auto &rm = RecipeManager::getInstance();
    const int xOffset = 3;
    const int yOffset = 3;

    for (int i = 0; i < rm.recipes.size(); ++i) {
        auto recipe = *rm.recipes.at(i);
        Color bColor = getColor("black");

        if (i == chosenRecipe)
            bColor = getColor("blue");

        font.drawText(recipe.nameOfProduct, xOffset, yOffset + i, getColor("white"), bColor);
    }

    auto &ingredients = rm.recipes[chosenRecipe]->ingredients;
    for (int i = 0; i < ingredients.size() + 1; ++i) {
        Color bColor = getColor("black");

        if ((layer == CraftingLayer::INGREDIENT || layer == CraftingLayer::MATERIAL) && i == chosenIngredient)
            bColor = getColor("blue");

        if (i != ingredients.size()) {
            auto ingredient = ingredients.at(i);
            if (currentRecipe != nullptr)
                ingredient = currentRecipe->ingredients[i];

            font.drawText(std::to_string(ingredient.quantity) + "x " + ingredient.entityType, xOffset + 14, yOffset + i,
                          getColor("white"), bColor);
        } else {
            Color fColor = getColor("grey");

            if (currentRecipeSatisfied())
                fColor = getColor("white");

            font.drawText("Construct", xOffset + 14, yOffset + i, fColor, bColor);
        }
    }

    if (layer == CraftingLayer::INGREDIENT || layer == CraftingLayer::MATERIAL) {
        std::vector<std::shared_ptr<Entity>> inventoryMaterials = filterInventoryForChosenMaterials();

        for (int i = 0; i < inventoryMaterials.size(); ++i) {
            Color bColor;
            auto &material = inventoryMaterials.at(i);

            if (layer == CraftingLayer::MATERIAL && i == chosenMaterial) {
                bColor = getColor("blue");
                font.drawText(material->graphic + " " + material->name, xOffset + 24, yOffset + i, bColor);
            } else
                font.drawText(material->graphic + " " + material->name, xOffset + 24, yOffset + i);
        }
    }

    if (createdMessageTimer-- > 0) {
        auto alpha = static_cast<int>(static_cast<float>(createdMessageTimer) / SHOW_CREATED_DISPLAY_LENGTH * 0xFF);
        font.drawText("You created a " + createdMessage, 3, SCREEN_HEIGHT - 2, alpha);
    }
}

std::vector<std::shared_ptr<Entity>> CraftingScreen::filterInventoryForChosenMaterials() {
    auto &rm = RecipeManager::getInstance();

    std::vector<std::shared_ptr<Entity>> inventoryMaterials;
    auto inventory = player.getInventory();
    std::copy_if(inventory.cbegin(), inventory.cend(), std::back_inserter(inventoryMaterials),
    [this, &rm] (auto &a) {
        if (!a->hasBehaviour("CraftingMaterialBehaviour"))
            return false;
        if (std::find(currentlyChosenMaterials.begin(), currentlyChosenMaterials.end(), a->ID) != currentlyChosenMaterials.end())
            return false;

        auto &b = dynamic_cast<CraftingMaterialBehaviour&>(*a->getBehaviourByID("CraftingMaterialBehaviour"));
        return rm.recipes[chosenRecipe]->ingredients[chosenIngredient].entityType == b.type;
    });

    return inventoryMaterials;
}

bool CraftingScreen::currentRecipeSatisfied() {
    return (currentRecipe != nullptr &&
            std::all_of(currentRecipe->ingredients.begin(), currentRecipe->ingredients.end(),
                    [] (auto &a) { return a.quantity == 0; }));
}

void CraftingScreen::reset() {
    currentlyChosenMaterials.clear();
    currentRecipe = nullptr;
    chosenRecipe = 0;
    chosenIngredient = 0;
    chosenMaterial = 0;
    layer = CraftingLayer::RECIPE;
    choosingPositionInWorld = false;
    haveChosenPositionInWorld = false;
    couldNotBuildAtPosition = false;
}

void CraftingScreen::buildItem(Point pos) {
    auto recipe = RecipeManager::getInstance().recipes[chosenRecipe];

    if (!recipe->goesIntoInventory) {
        recipe->pointIfNotGoingIntoInventory = pos;
        enabled = false;
    }

    recipe->produce();

    for (const auto &ID : currentlyChosenMaterials) {
        player.removeFromInventory(ID);
        EntityManager::getInstance().eraseByID(ID);
    }
    createdMessage = currentRecipe->nameOfProduct;
    createdMessageTimer = SHOW_CREATED_DISPLAY_LENGTH;
}

void CraftingScreen::enable() {
    enabled = true;
    reset();
}

void EquipmentScreen::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            if (choosingEquipmentAction)
                choosingEquipmentAction = false;
            else if (choosingNewEquipment)
                choosingNewEquipment = false;
            else
                enabled = false;
            break;
        case SDLK_j:
            if (choosingNewEquipment) {
                auto equippableIDs = player.getInventoryItemsEquippableInSlot(chosenSlot);
                if (choosingNewEquipmentIndex == equippableIDs.size() - 1)
                    choosingNewEquipmentIndex = 0;
                else
                    ++choosingNewEquipmentIndex;
            } else
                ++chosenSlot;
            break;
        case SDLK_k:
            if (choosingNewEquipment) {
                auto equippableIDs = player.getInventoryItemsEquippableInSlot(chosenSlot);
                if (choosingNewEquipmentIndex == 0)
                    choosingNewEquipmentIndex = static_cast<int>(equippableIDs.size()) - 1;
                else
                    --choosingNewEquipmentIndex;
            } else
                --chosenSlot;
            break;
        case SDLK_RETURN:
            if (choosingNewEquipment) {
                auto equippableIDs = player.getInventoryItemsEquippableInSlot(chosenSlot);
                player.equip(chosenSlot, equippableIDs[choosingNewEquipmentIndex]);
                reset();
            } else if (choosingEquipmentAction) {
                if (player.hasEquippedInSlot(chosenSlot)) {
                    player.unequip(chosenSlot);
                } else {
                    choosingNewEquipment = true;
                }
                choosingEquipmentAction = false;
                choosingEquipmentActionIndex = 0;
            } else if (!player.hasEquippedInSlot(chosenSlot))
                choosingNewEquipment = true;
            else
                choosingEquipmentAction = true;
            break;
    }
}

void EquipmentScreen::render(Font &font) {
    int y = 3;

    for (auto slot : EQUIPMENT_SLOTS) {
        Color bColor = getColor("black");
        if (chosenSlot == slot)
            bColor = getColor("blue");

        std::string currentlyEquipped = player.getEquipmentID(slot);
        if (!currentlyEquipped.empty()) {
            auto e = EntityManager::getInstance().getEntityByID(currentlyEquipped);
            font.drawText(e->graphic + " " + e->name, 20, y);
        }

        font.drawText(slotToString(slot), 6, y++, bColor);
    }

    if (choosingEquipmentAction) {
        std::vector<std::string> lines;
        if (player.hasEquippedInSlot(chosenSlot))
            lines.emplace_back("$(right)Unequip");
        else
            lines.emplace_back("$(right)Equip");

        showMessageBox(font, lines, 1, SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10);
    }

    if (choosingNewEquipment) {
        auto equippableIDs = player.getInventoryItemsEquippableInSlot(chosenSlot);
        std::vector<std::string> lines;

        for (int i = 0; i < equippableIDs.size(); ++i) {
            auto ID = equippableIDs[i];
            auto entity = EntityManager::getInstance().getEntityByID(ID);

            lines.emplace_back((i == choosingNewEquipmentIndex ? "$(right)" : " ") + entity->graphic + " " + entity->name);
        }

        showMessageBox(font, lines, 1, SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10);
    }
}

void EquipmentScreen::enable() {
    enabled = true;
    choosingEquipmentAction = false;
    choosingEquipmentActionIndex = 0;
    choosingNewEquipment = false;
    choosingNewEquipmentIndex = 0;
}

void EquipmentScreen::reset() {
    choosingNewEquipment = false;
    choosingEquipmentAction = false;
    choosingNewEquipmentIndex = 0;
    choosingEquipmentActionIndex = 0;
}

