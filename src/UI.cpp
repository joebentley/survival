#include "UI.h"
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

void MessageBoxRenderer::queueMessageBox(const std::vector<std::string> &contents, int padding, int x, int y) {
    mRenderingQueue.push_back(MessageBoxData {contents, padding, x, y});
}

void MessageBoxRenderer::queueMessageBoxCentered(const std::vector<std::string> &contents, int padding) {
    std::vector<int> lineLengths;
    std::transform(contents.cbegin(), contents.cend(), std::back_inserter(lineLengths),
            [] (const auto &a) { return getFontStringLength(a); });
    const int maxLength = *std::max_element(lineLengths.cbegin(), lineLengths.cend());
    const int width = maxLength + 2 * padding;
    const int height = static_cast<int>(contents.size()) + 2 * padding;

    queueMessageBox(contents, padding, (SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
}

void MessageBoxRenderer::render(Font &font) {
    while (!mRenderingQueue.empty()) {
        auto data = mRenderingQueue.front();
        mRenderingQueue.pop_front();

        showMessageBox(font, data.mContents, data.mPadding, data.mX, data.mY);
    }
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
    font.drawText(item.mGraphic + " " + item.mName, InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET);
    font.drawText(item.mShortDesc, InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 2);

    auto words = wordWrap(item.mLongDesc, InventoryScreen::WORD_WRAP_COLUMN);
    for (int i = 0; i < words.size(); ++i) {
        font.drawText(words[i], InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 4 + i);
    }

    int y = 2;
    const int yOffset = InventoryScreen::Y_OFFSET + 4 + static_cast<int>(words.size());

    auto b = item.getBehaviourByID("PickuppableBehaviour");
    if (b != nullptr) {
        int weight = dynamic_cast<PickuppableBehaviour &>(*b).weight;
        font.drawText("It weighs " + std::to_string(weight) + (weight == 1 ? " pound" : " pounds"),
                      InventoryScreen::X_OFFSET, yOffset + y++);
    }

    b = item.getBehaviourByID("MeleeWeaponBehaviour");
    if (b != nullptr) {
        int damage = dynamic_cast<MeleeWeaponBehaviour &>(*b).extraDamage;
        font.drawText("It adds $[red]" + std::to_string(damage) + "$[white] to your damage roll",
                InventoryScreen::X_OFFSET, yOffset + y++);
    }

    b = item.getBehaviourByID("AdditionalCarryWeightBehaviour");
    if (b != nullptr) {
        int carry = dynamic_cast<AdditionalCarryWeightBehaviour &>(*b).additionalCarryWeight;
        font.drawText("It adds an extra " + std::to_string(carry) + "lb to your max carry weight",
                InventoryScreen::X_OFFSET, yOffset + y++);
    }

    b = item.getBehaviourByID("EquippableBehaviour");
    if (b != nullptr) {
        auto slots = dynamic_cast<EquippableBehaviour &>(*b).getEquippableSlots();
        std::string slotsString;

        for (auto slot = slots.cbegin(); slot != slots.cend(); ++slot) {
            slotsString += slotToString(*slot);
            if (slot != slots.cend() - 1)
                slotsString += ", ";
        }

        font.drawText("Can be equipped in: " + slotsString,
                InventoryScreen::X_OFFSET, yOffset + y++);
    }

    b = item.getBehaviourByID("HealingItemBehaviour");
    if (b != nullptr) {
        float healing = dynamic_cast<HealingItemBehaviour &>(*b).healingAmount;
        font.drawText("${white}$[red]+${black}$[white] Can be used to heal for " + std::to_string(healing),
                InventoryScreen::X_OFFSET, yOffset + y++);
    }

    b = item.getBehaviourByID("WaterContainerBehaviour");
    if (b != nullptr) {
        auto &casted = dynamic_cast<WaterContainerBehaviour &>(*b);
        int current = casted.getAmount();
        font.drawText("It can hold $[cyan]" + std::to_string(casted.getMaxCapacity()) + "$[white] drams of water." +
                      (current > 0 ? " It currently holds $[cyan]" + std::to_string(current) + "$[white] drams of water." : ""),
                InventoryScreen::X_OFFSET, yOffset + y++);
    }

    font.drawText("esc-back", 1, SCREEN_HEIGHT - 2);
}

void InventoryScreen::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }
}

void InventoryScreen::render(Font &font) {
    if (mViewingDescription) {
        drawDescriptionScreen(font, *mPlayer.getInventoryItem(mChosenIndex));
        return;
    }

    font.draw("right", X_OFFSET - 1, mChosenIndex + Y_OFFSET);

    for (int i = 0; i < mPlayer.getInventorySize(); ++i) {
        auto item = mPlayer.getInventoryItem(i);
        std::string displayString = item->mGraphic + " " + item->mName;

        font.drawText(displayString, X_OFFSET, i + Y_OFFSET);
        if (mPlayer.hasEquipped(item->mID))
            font.drawText("(" + slotToString(mPlayer.getEquipmentSlotByID(item->mID)) + ")",
                    X_OFFSET + getFontStringLength(displayString) + 2, i + Y_OFFSET);
    }

    std::string colorStr;
    float hpPercent = mPlayer.mHp / mPlayer.mMaxHp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(mPlayer.mHp))
                  + "/" + std::to_string((int)ceil(mPlayer.mMaxHp)), SCREEN_WIDTH - X_STATUS_OFFSET, 1);

    if (mPlayer.hunger > 0.7)
        font.drawText("${black}$[green]sated", SCREEN_WIDTH - X_STATUS_OFFSET, 2);
    else if (mPlayer.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", SCREEN_WIDTH - X_STATUS_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", SCREEN_WIDTH - X_STATUS_OFFSET, 2);

    font.drawText("${black}" + std::to_string(mPlayer.getCarryingWeight()) + "/" + std::to_string(mPlayer.getMaxCarryWeight()) + "lb",
                  SCREEN_WIDTH - X_STATUS_OFFSET, 4);

    std::string helpString;
    auto item = mPlayer.getInventoryItem(mChosenIndex);
    if (item->hasBehaviour("EatableBehaviour"))
        helpString += "e-eat  ";
    if (item->hasBehaviour("ApplyableBehaviour"))
        helpString += "a-apply  ";
    font.drawText(helpString + "d-drop  return-view desc  esc-exit inv", 1, SCREEN_HEIGHT - 2);
}

PlayerEntity &InventoryScreen::getPlayer() const {
    return mPlayer;
}

int InventoryScreen::getChosenIndex() const {
    return mChosenIndex;
}

void InventoryScreen::setChosenIndex(int chosenIndex) {
    mChosenIndex = chosenIndex;
}

bool InventoryScreen::isViewingDescription() const {
    return mViewingDescription;
}

void InventoryScreen::setViewingDescription(bool viewingDescription) {
    mViewingDescription = viewingDescription;
}

void LootingDialog::showItemsToLoot(std::vector<Entity *> items) {
    mItemsToShow = std::move(items);
    mEnabled = true;
}

void LootingDialog::showItemsToLoot(std::vector<Entity *> items, Entity *entityToTransferFrom)
{
    mItemsToShow = std::move(items);
    this->mEntityToTransferFrom = std::move(entityToTransferFrom);
    mEnabled = true;
}

void LootingDialog::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }

    if (mViewingDescription)
        mShouldRenderWorld = false;
    else
        mShouldRenderWorld = true;
}

void LootingDialog::render(Font &font) {
    if (mShowingTooMuchWeightMessage) {
        const std::string& displayString = "You cannot carry that much!";
        MessageBoxRenderer::getInstance().queueMessageBoxCentered(displayString, 1);
    }

    if (mViewingDescription) {
        drawDescriptionScreen(font, *mItemsToShow[mChosenIndex]);
        return;
    }

    auto numItems = (int)mItemsToShow.size();

    if (numItems == 0) {
        mEnabled = false;
        mEntityToTransferFrom = nullptr;
        return;
    }

    const int x = SCREEN_WIDTH / 2 - (DIALOG_WIDTH + 4) / 2;
    const int y = 10;

    font.drawText("${black}$(p23)" + repeat(DIALOG_WIDTH + 4, "$(p27)") + "$(p9)", x, y);
    font.drawText("${black}$(p8)" + std::string(DIALOG_WIDTH + 4, ' ') + "$(p8)", x, y+1);

    for (int i = 0; i < numItems; ++i) {
        auto item = mItemsToShow[i];
        int weight = dynamic_cast<PickuppableBehaviour&>(*item->getBehaviourByID("PickuppableBehaviour")).weight;

        std::string weightString = std::to_string(weight);
        std::string string = item->mGraphic + " " + item->mName.substr(0, DIALOG_WIDTH - 6);
        string += std::string(DIALOG_WIDTH - getFontStringLength(string) - 3 - weightString.size() + 1, ' ') + "$[white]" + weightString + " lb";

        font.drawText("${black}$(p8)  " + string + "${black} $[white]$(p8)", x, y+2+i);
    }

    font.draw("right", x + 2, y + 2 + mChosenIndex);

    font.drawText("${black}$(p8)" + std::string(DIALOG_WIDTH + 4, ' ') + "$(p8)", x, y+numItems+2);
    std::string string = "g-loot  esc-quit  return-desc";
    font.drawText("${black}$(p8)  " + string + std::string(DIALOG_WIDTH - string.size() + 2, ' ') + "$(p8)", x, y+numItems+3);
    font.drawText("${black}$(p22)" + repeat(DIALOG_WIDTH + 4, "$(p27)") + "$(p10)", x, y+numItems+4);
}

bool LootingDialog::isViewingDescription() const {
    return mViewingDescription;
}

void LootingDialog::setViewingDescription(bool viewingDescription) {
    mViewingDescription = viewingDescription;
}

bool LootingDialog::isShowingTooMuchWeightMessage() const {
    return mShowingTooMuchWeightMessage;
}

void LootingDialog::setShowingTooMuchWeightMessage(bool showingTooMuchWeightMessage) {
    mShowingTooMuchWeightMessage = showingTooMuchWeightMessage;
}

int LootingDialog::getChosenIndex() const {
    return mChosenIndex;
}

void LootingDialog::setChosenIndex(int chosenIndex) {
    mChosenIndex = chosenIndex;
}

PlayerEntity &LootingDialog::getPlayer() const {
    return mPlayer;
}

std::vector<Entity *> & LootingDialog::getItemsToShow() {
    return mItemsToShow;
}

Entity *LootingDialog::getEntityToTransferFrom() const {
    return mEntityToTransferFrom;
}

inline Point InspectionDialog::clipToScreenEdge(const Point &p) const {
    auto worldPos = mPlayer.getWorldPos();
    auto point = p - Point(SCREEN_WIDTH, SCREEN_HEIGHT) * worldPos;
    Point returnPoint(p);
    if (point.mX < 0)
        returnPoint.mX = worldPos.mX * SCREEN_WIDTH;
    else if (point.mX > SCREEN_WIDTH - 1)
        returnPoint.mX = (worldPos.mX + 1) * SCREEN_WIDTH - 1;
    if (point.mY < 0)
        returnPoint.mY = worldPos.mY * SCREEN_HEIGHT;
    else if (point.mY > SCREEN_HEIGHT - 1)
        returnPoint.mY = (worldPos.mY + 1) * SCREEN_HEIGHT - 1;
    return returnPoint;
}

void InspectionDialog::handleInput(SDL_KeyboardEvent &e) {
    Point posOffset;

    switch (e.keysym.sym) {
        case SDLK_h:
            if (!mViewingDescription)
                posOffset = Point(-1, 0);
            break;
        case SDLK_j:
            if (!mViewingDescription)
                posOffset = Point(0, 1);
            break;
        case SDLK_k:
            if (!mViewingDescription)
                posOffset = Point(0, -1);
            break;
        case SDLK_l:
            if (!mViewingDescription)
                posOffset = Point(1, 0);
            break;
        case SDLK_y:
            if (!mViewingDescription)
                posOffset = Point(-1, -1);
            break;
        case SDLK_u:
            if (!mViewingDescription)
                posOffset = Point(1, -1);
            break;
        case SDLK_b:
            if (!mViewingDescription)
                posOffset = Point(-1, 1);
            break;
        case SDLK_n:
            if (!mViewingDescription)
                posOffset = Point(1, 1);
            break;
        case SDLK_EQUALS:
            if (mSelectingFromMultipleOptions && !mViewingDescription) {
                const auto &currentEntities = EntityManager::getInstance().getEntitiesAtPosFaster(mChosenPoint);
                if (mChosenIndex == currentEntities.size() - 1)
                    mChosenIndex = 0;
                else
                    mChosenIndex++;
            }
            break;
        case SDLK_MINUS:
            if (mSelectingFromMultipleOptions && !mViewingDescription) {
                const auto &currentEntities = EntityManager::getInstance().getEntitiesAtPosFaster(mChosenPoint);
                if (mChosenIndex == 0)
                    mChosenIndex = (int)(currentEntities.size() - 1);
                else
                    mChosenIndex--;
            }
            break;
        case SDLK_RETURN:
            if (mThereIsAnEntity)
                mViewingDescription = true;
            break;
        case SDLK_ESCAPE:
            if (mViewingDescription)
                mViewingDescription = false;
            else
                mEnabled = false;
            break;
        default:
            break;
    }

    if (e.keysym.mod & KMOD_SHIFT)
        posOffset *= 5;

    mChosenPoint = clipToScreenEdge(mChosenPoint + posOffset);

    if (mViewingDescription)
        mShouldRenderWorld = false;
    else
        mShouldRenderWorld = true;
}

void InspectionDialog::render(Font &font) {
    const auto &entitiesAtPointBefore = EntityManager::getInstance().getEntitiesAtPosFaster(mChosenPoint);
    std::vector<Entity *> entitiesAtPoint;

    std::copy_if(entitiesAtPointBefore.cbegin(), entitiesAtPointBefore.cend(), std::back_inserter(entitiesAtPoint),
    [] (auto &a) { return !a->mIsInAnInventory; });

    if (mViewingDescription) {
        drawDescriptionScreen(font, *entitiesAtPoint[mChosenIndex]);
        return;
    }

    const auto &chosenPointScreen = worldToScreen(mChosenPoint);
    font.drawText("${black}$[yellow]X", chosenPointScreen);

    int xPosWindow = chosenPointScreen.mX >= SCREEN_WIDTH / 2 ? 2 : SCREEN_WIDTH / 2 + 1;

    if (entitiesAtPoint.size() > 1) {
        mSelectingFromMultipleOptions = true;
        std::vector<std::string> lines;

        lines.emplace_back(" You see");
        std::transform(entitiesAtPoint.cbegin(), entitiesAtPoint.cend(), std::back_inserter(lines),
                       [] (auto &a) -> std::string { return " " + a->mGraphic + " " + a->mName; });

        lines.emplace_back("");
        lines.emplace_back(" (-)-$(up) (=)-$(down) return-desc");
        showMessageBox(font, lines, 1, xPosWindow - 1, 2);
        font.draw("right", xPosWindow + 2 - 1, 2 + 3 + mChosenIndex);
        mThereIsAnEntity = true;
    } else {
        mSelectingFromMultipleOptions = false;
        mChosenIndex = 0;
    }

    if (entitiesAtPoint.size() == 1) {
        const auto &entity = *entitiesAtPoint[0];

        std::vector<std::string> lines;
        lines.emplace_back(entity.mGraphic + " " + entity.mName);

        if (!entity.mShortDesc.empty()) {
            lines.emplace_back("");
            auto descLines = wordWrap(entity.mShortDesc, SCREEN_WIDTH / 2 - 5);
            std::copy(descLines.begin(), descLines.end(), std::back_inserter(lines));
        }

        if (!entity.mLongDesc.empty()) {
            lines.emplace_back("");
            auto descLines = wordWrap(entity.mLongDesc, SCREEN_WIDTH / 2 - 5);
            std::copy(descLines.begin(), descLines.end(), std::back_inserter(lines));
        }

        const int cappedNumLines = SCREEN_HEIGHT - 20;
        std::vector<std::string> linesCapped = lines;
        if (lines.size() > cappedNumLines) {
            linesCapped = std::vector<std::string>(lines.begin(), lines.begin() + cappedNumLines);
            *(linesCapped.end() - 1) += "...";
        }

        MessageBoxRenderer::getInstance().queueMessageBox(lines, 1, xPosWindow, 2);

        mThereIsAnEntity = true;
    }

    if (entitiesAtPoint.empty())
        mThereIsAnEntity = false;
}

void CraftingScreen::handleInput(SDL_KeyboardEvent &e) {
    auto &rm = RecipeManager::getInstance();
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            mEnabled = false;
            break;
        case SDLK_j:
            if (mChoosingPositionInWorld) {
                tryToBuildAtPosition(Point {0, 1});
                break;
            }

            if (mLayer == CraftingLayer::RECIPE) {
                if (mChosenRecipe == rm.mRecipes.size() - 1)
                    mChosenRecipe = 0;
                else
                    mChosenRecipe++;
                mChosenIngredient = 0;
                mChosenMaterial = 0;
            } else if (mLayer == CraftingLayer::INGREDIENT) {
                if (mChosenIngredient == rm.mRecipes[mChosenRecipe]->mIngredients.size())
                    mChosenIngredient = 0;
                else
                    mChosenIngredient++;
                mChosenMaterial = 0;
            } else if (mLayer == CraftingLayer::MATERIAL) {
                auto inventoryMaterials = filterInventoryForChosenMaterials();
                if (mChosenMaterial == inventoryMaterials.size() - 1)
                    mChosenMaterial = 0;
                else
                    mChosenMaterial++;
            }
            break;
        case SDLK_k:
            if (mChoosingPositionInWorld) {
                tryToBuildAtPosition(Point {0, -1});
                break;
            }

            if (mLayer == CraftingLayer::RECIPE) {
                if (mChosenRecipe == 0)
                    mChosenRecipe = (int) rm.mRecipes.size() - 1;
                else
                    mChosenRecipe--;
                mChosenIngredient = 0;
                mChosenMaterial = 0;
            } else if (mLayer == CraftingLayer::INGREDIENT) {
                if (mChosenIngredient == 0)
                    mChosenIngredient = (int)rm.mRecipes[mChosenRecipe]->mIngredients.size();
                else
                    mChosenIngredient--;
                mChosenMaterial = 0;
            } else if (mLayer == CraftingLayer::MATERIAL) {
                std::vector<Entity *> inventoryMaterials = filterInventoryForChosenMaterials();
                if (mChosenMaterial == 0)
                    mChosenMaterial = (int)inventoryMaterials.size() - 1;
                else
                    mChosenMaterial--;
            }
            break;
        case SDLK_l:
            if (mChoosingPositionInWorld) {
                tryToBuildAtPosition(Point {1, 0});
                break;
            }
        case SDLK_RETURN:
            if (mLayer == CraftingLayer::RECIPE) {
                mChosenIngredient = 0;
                mChosenMaterial = 0;
                mLayer = CraftingLayer::INGREDIENT;

                mCurrentRecipe = std::make_unique<Recipe>(Recipe(*rm.mRecipes[mChosenRecipe]));
            } else if (mLayer == CraftingLayer::INGREDIENT) {
                if (mChosenIngredient == mCurrentRecipe->mIngredients.size()) {
                    if (currentRecipeSatisfied()) {
                        if (rm.mRecipes[mChosenRecipe]->mGoesIntoInventory || mHaveChosenPositionInWorld) {
                            buildItem(Point {0, 0});
                            this->reset();
                        } else {
                            // Get player to build object into the world
                            mChoosingPositionInWorld = true;
                            break;
                        }
                    }
                } else if (mCurrentRecipe->mIngredients[mChosenIngredient].mQuantity > 0 && !filterInventoryForChosenMaterials().empty())
                    mLayer = CraftingLayer::MATERIAL;
            } else if (mLayer == CraftingLayer::MATERIAL) {
                auto inventoryMaterials = filterInventoryForChosenMaterials();

                if (mCurrentRecipe->mIngredients[mChosenIngredient].mQuantity > 0) {
                    mCurrentlyChosenMaterials.emplace_back(inventoryMaterials[mChosenMaterial]->mID);
                    mCurrentRecipe->mIngredients[mChosenIngredient].mQuantity--;
                }
                // Have finished this material requirement
                if (mCurrentRecipe->mIngredients[mChosenIngredient].mQuantity == 0) {
                    mLayer = CraftingLayer::INGREDIENT;
                    mChosenIngredient++; // Automatically go to next ingredient for faster crafting
                }
                // This means we have now ran out of the desired material and should ensure that we leave the current crafting layer
                else if (inventoryMaterials.size() == 1) {
                    mLayer = CraftingLayer::INGREDIENT;
                }
            }
            break;
        case SDLK_h:
            if (mChoosingPositionInWorld) {
                tryToBuildAtPosition(Point {-1, 0});
                break;
            }
        case SDLK_BACKSPACE:
            if (mLayer == CraftingLayer::INGREDIENT) {
                mChosenMaterial = 0;
                mLayer = CraftingLayer::RECIPE;
                mCurrentRecipe.release();
                mCurrentlyChosenMaterials.clear();
            } else if (mLayer == CraftingLayer::MATERIAL) {
                mChosenMaterial = 0;
                mLayer = CraftingLayer::INGREDIENT;
            }
            break;
        case SDLK_y:
            if (mChoosingPositionInWorld)
                tryToBuildAtPosition(Point {-1, -1});
            break;
        case SDLK_u:
            if (mChoosingPositionInWorld)
                tryToBuildAtPosition(Point {1, -1});
            break;
        case SDLK_b:
            if (mChoosingPositionInWorld)
                tryToBuildAtPosition(Point {-1, 1});
            break;
        case SDLK_n:
            if (mChoosingPositionInWorld)
                tryToBuildAtPosition(Point {1, 1});
            break;
    }

    if (mChoosingPositionInWorld)
        mShouldRenderWorld = true;
    else
        mShouldRenderWorld = false;
}

void CraftingScreen::tryToBuildAtPosition(Point posOffset) {
    auto p = posOffset + mPlayer.getPos();
    if (EntityManager::getInstance().getEntitiesAtPosFaster(p).empty()) {
        mHaveChosenPositionInWorld = true;
        mChoosingPositionInWorld = false;
        buildItem(p);
    } else {
        mCouldNotBuildAtPosition = true;
    }
}

void CraftingScreen::render(Font &font) {
    if (mChoosingPositionInWorld) {
        auto player = EntityManager::getInstance().getEntityByID("Player");

        int y = 0;
        if (worldToScreen(player->getPos()).mY < SCREEN_HEIGHT / 2)
            y = SCREEN_HEIGHT - 1;

        std::string message = "Choose direction to place object";
        if (mCouldNotBuildAtPosition)
            message = "Please choose a square that does not already have an entity on";

        font.drawText(message, 1, y, FontColor::getColor("white"), FontColor::getColor("black"));
        return;
    }

    auto &rm = RecipeManager::getInstance();
    const int xOffset = 3;
    const int yOffset = 3;

    for (int i = 0; i < rm.mRecipes.size(); ++i) {
        auto recipe = *rm.mRecipes.at(i);
        Color bColor = FontColor::getColor("black");

        if (i == mChosenRecipe)
            bColor = FontColor::getColor("blue");

        font.drawText(recipe.mNameOfProduct, xOffset, yOffset + i, FontColor::getColor("white"), bColor);
    }

    auto &ingredients = rm.mRecipes[mChosenRecipe]->mIngredients;
    for (int i = 0; i < ingredients.size() + 1; ++i) {
        Color bColor = FontColor::getColor("black");

        if ((mLayer == CraftingLayer::INGREDIENT || mLayer == CraftingLayer::MATERIAL) && i == mChosenIngredient)
            bColor = FontColor::getColor("blue");

        if (i != ingredients.size()) {
            auto ingredient = ingredients.at(i);
            if (mCurrentRecipe != nullptr)
                ingredient = mCurrentRecipe->mIngredients[i];

            font.drawText(std::to_string(ingredient.mQuantity) + "x " + ingredient.mEntityType, xOffset + 14, yOffset + i,
                    FontColor::getColor("white"), bColor);
        } else {
            Color fColor = FontColor::getColor("grey");

            if (currentRecipeSatisfied())
                fColor = FontColor::getColor("white");

            font.drawText("Construct", xOffset + 14, yOffset + i, fColor, bColor);
        }
    }

    if (mLayer == CraftingLayer::INGREDIENT || mLayer == CraftingLayer::MATERIAL) {
        std::vector<Entity *> inventoryMaterials = filterInventoryForChosenMaterials();

        for (int i = 0; i < inventoryMaterials.size(); ++i) {
            Color bColor;
            auto &material = inventoryMaterials.at(i);

            if (mLayer == CraftingLayer::MATERIAL && i == mChosenMaterial) {
                bColor = FontColor::getColor("blue");
                font.drawText(material->mGraphic + " " + material->mName, xOffset + 24, yOffset + i, bColor);
            } else
                font.drawText(material->mGraphic + " " + material->mName, xOffset + 24, yOffset + i);
        }
    }
}

std::vector<Entity *> CraftingScreen::filterInventoryForChosenMaterials() {
    auto &rm = RecipeManager::getInstance();

    std::vector<Entity *> inventoryMaterials;
    auto inventory = mPlayer.getInventory();
    std::copy_if(inventory.cbegin(), inventory.cend(), std::back_inserter(inventoryMaterials),
    [this, &rm] (auto &a) {
        if (!a->hasBehaviour("CraftingMaterialBehaviour"))
            return false;
        if (std::find(mCurrentlyChosenMaterials.begin(), mCurrentlyChosenMaterials.end(), a->mID) != mCurrentlyChosenMaterials.end())
            return false;

        auto &b = dynamic_cast<CraftingMaterialBehaviour&>(*a->getBehaviourByID("CraftingMaterialBehaviour"));
        return rm.mRecipes[mChosenRecipe]->mIngredients[mChosenIngredient].mEntityType == b.type;
    });

    return inventoryMaterials;
}

bool CraftingScreen::currentRecipeSatisfied() {
    return (mCurrentRecipe != nullptr &&
            std::all_of(mCurrentRecipe->mIngredients.begin(), mCurrentRecipe->mIngredients.end(),
                    [] (auto &a) { return a.mQuantity == 0; }));
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
}

void CraftingScreen::buildItem(Point pos) {
    auto recipe = RecipeManager::getInstance().mRecipes[mChosenRecipe];

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

void EquipmentScreen::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            if (mChoosingEquipmentAction)
                mChoosingEquipmentAction = false;
            else if (mChoosingNewEquipment)
                mChoosingNewEquipment = false;
            else
                mEnabled = false;
            break;
        case SDLK_j:
            if (mChoosingNewEquipment) {
                auto equippableIDs = mPlayer.getInventoryItemsEquippableInSlot(mChosenSlot);
                if (mChoosingNewEquipmentIndex == equippableIDs.size() - 1)
                    mChoosingNewEquipmentIndex = 0;
                else
                    ++mChoosingNewEquipmentIndex;
            } else if (!mChoosingEquipmentAction)
                ++mChosenSlot;
            break;
        case SDLK_k:
            if (mChoosingNewEquipment) {
                auto equippableIDs = mPlayer.getInventoryItemsEquippableInSlot(mChosenSlot);
                if (mChoosingNewEquipmentIndex == 0)
                    mChoosingNewEquipmentIndex = static_cast<int>(equippableIDs.size()) - 1;
                else
                    --mChoosingNewEquipmentIndex;
            } else if (!mChoosingEquipmentAction)
                --mChosenSlot;
            break;
        case SDLK_RETURN:
            if (mChoosingNewEquipment) {
                auto equippableIDs = mPlayer.getInventoryItemsEquippableInSlot(mChosenSlot);
                mPlayer.equip(mChosenSlot, equippableIDs[mChoosingNewEquipmentIndex]);
                reset();
            } else if (mChoosingEquipmentAction) {
                if (mPlayer.hasEquippedInSlot(mChosenSlot)) {
                    mPlayer.unequip(mChosenSlot);
                } else if (!mPlayer.getInventoryItemsEquippableInSlot(mChosenSlot).empty()) {
                    mChoosingNewEquipment = true;
                }
                mChoosingEquipmentAction = false;
                mChoosingEquipmentActionIndex = 0;
            } else if (!mPlayer.hasEquippedInSlot(mChosenSlot) && !mPlayer.getInventoryItemsEquippableInSlot(mChosenSlot).empty())
                mChoosingNewEquipment = true;
            else
                mChoosingEquipmentAction = true;
            break;
    }
}

void EquipmentScreen::render(Font &font) {
    int y = 3;

    for (auto slot : EQUIPMENT_SLOTS) {
        Color bColor = FontColor::getColor("black");
        if (mChosenSlot == slot)
            bColor = FontColor::getColor("blue");

        std::string currentlyEquipped = mPlayer.getEquipmentID(slot);
        if (!currentlyEquipped.empty()) {
            auto e = EntityManager::getInstance().getEntityByID(currentlyEquipped);
            font.drawText(e->mGraphic + " " + e->mName +
                          (slot == EquipmentSlot::RIGHT_HAND ?
                           " $[red]$(heart)$[white]" + std::to_string(mPlayer.mHitTimes)
                           + "d" + std::to_string(mPlayer.computeMaxDamage()) : "")
                    , 20, y);
        }

        font.drawText(slotToString(slot), 6, y++, bColor);
    }

    if (mChoosingEquipmentAction) {
        std::vector<std::string> lines;
        if (mPlayer.hasEquippedInSlot(mChosenSlot))
            lines.emplace_back("$(right)Unequip");
        else
            lines.emplace_back("$(right)Equip");

        MessageBoxRenderer::getInstance().queueMessageBoxCentered(lines, 1);
    }

    if (mChoosingNewEquipment) {
        auto equippableIDs = mPlayer.getInventoryItemsEquippableInSlot(mChosenSlot);
        std::vector<std::string> lines;

        for (int i = 0; i < equippableIDs.size(); ++i) {
            auto ID = equippableIDs[i];
            auto entity = EntityManager::getInstance().getEntityByID(ID);

            lines.emplace_back((i == mChoosingNewEquipmentIndex ? "$(right)" : " ") + entity->mGraphic + " " + entity->mName);

            if (mChosenSlot == EquipmentSlot::RIGHT_HAND) {
                auto b = entity->getBehaviourByID("MeleeWeaponBehaviour");
                if (b != nullptr) {
                    lines[i] += " $[red]$(heart)$[white]" + std::to_string(mPlayer.mHitTimes)
                                + "d" +
                                std::to_string(mPlayer.mHitAmount + dynamic_cast<MeleeWeaponBehaviour &>(*b).extraDamage);
                }
            }
        }

        MessageBoxRenderer::getInstance().queueMessageBoxCentered(lines, 1);
    }
}

void EquipmentScreen::enable() {
    mEnabled = true;
    mChoosingEquipmentAction = false;
    mChoosingEquipmentActionIndex = 0;
    mChoosingNewEquipment = false;
    mChoosingNewEquipmentIndex = 0;
}

void EquipmentScreen::reset() {
    mChoosingNewEquipment = false;
    mChoosingEquipmentAction = false;
    mChoosingNewEquipmentIndex = 0;
    mChoosingEquipmentActionIndex = 0;
}

void NotificationMessageRenderer::queueMessage(const std::string &message) {
    mMessagesToBeRendered.push_back(message);
    if (mMessagesToBeRendered.size() > cMaxOnScreen)
        mMessagesToBeRendered.pop_front();
    mAllMessages.push_back(message);
}

void NotificationMessageRenderer::render(Font &font) {
    auto currentTime = clock();

    auto front = mMessagesToBeRendered.cbegin();
    for (int i = 0; front != mMessagesToBeRendered.cend() && i < cMaxOnScreen; ++i, ++front) {
        font.drawText(*front, 4, cInitialYPos - static_cast<int>(mMessagesToBeRendered.size()) + i,
                i == 0 ? static_cast<int>(0xFF * mAlpha) : -1);

        if (i == 0) // TODO: why is alpha decay slower when in a menu?
            mAlpha -= cAlphaDecayPerSec * static_cast<float>(currentTime - previousTime) / CLOCKS_PER_SEC;
    }

    if (mAlpha <= 0) {
        mAlpha = 1;
        mMessagesToBeRendered.pop_front();
    }

    previousTime = currentTime;
}

void NotificationMessageScreen::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
        case SDLK_m:
            mEnabled = false;
            break;
    }
}

void NotificationMessageScreen::render(Font &font) {
    auto messages = NotificationMessageRenderer::getInstance().getMessages();

    const int numMessagesToShow = SCREEN_HEIGHT - 4;
    const int xOffset = 4;

    font.drawText("Message history", xOffset, 1);
    font.drawText("m or esc to exit", SCREEN_WIDTH - 20, 1);

    auto front = messages.crbegin();
    for (int i = 0; front != messages.crend() && i < numMessagesToShow; ++i, ++front) {
        font.drawText(*front, xOffset, numMessagesToShow + 1 - i);
    }
}

void HelpScreen::handleInput(SDL_KeyboardEvent &e) {
    if (e.keysym.sym == SDLK_ESCAPE || (e.keysym.sym == SDLK_SLASH && e.keysym.mod & KMOD_SHIFT))
        mEnabled = false;
}

void HelpScreen::render(Font &font) {
    const int yPadding = 2;
    const int xPadding = 2;
    int y = 0;

    for (auto line : cDisplayLines) {
        font.drawText(line, xPadding, yPadding + y++);
    }
}
