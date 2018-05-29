#include "UI.h"
#include "utils.h"
#include "Behaviours.h"
#include "World.h"

#include <sstream>
#include <algorithm>
#include <boost/any.hpp>

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

    {
        auto b = item.getProperty("Pickuppable");
        if (b != nullptr) {
            int weight = boost::any_cast<int>(b->getValue());
            font.drawText("It weighs " + std::to_string(weight) + (weight == 1 ? " pound" : " pounds"),
                    InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto b = item.getProperty("MeleeWeaponDamage");
        if (b != nullptr) {
            int damage = boost::any_cast<int>(b->getValue());
            font.drawText("It adds $[red]" + std::to_string(damage) + "$[white] to your damage roll",
                    InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto b = item.getProperty("AdditionalCarryWeight");
        if (b != nullptr) {
            int carry = boost::any_cast<int>(b->getValue());
            font.drawText("It adds an extra " + std::to_string(carry) + "lb to your max carry weight",
                    InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto equippable = item.getProperty("Equippable");
        if (equippable != nullptr) {
            auto b = boost::any_cast<EquippableProperty::Equippable>(equippable->getValue());
            auto slots = b.getEquippableSlots();
            std::string slotsString;

            for (auto slot = slots.cbegin(); slot != slots.cend(); ++slot) {
                slotsString += slotToString(*slot);
                if (slot != slots.cend() - 1)
                    slotsString += ", ";
            }

            font.drawText("Can be equipped in: " + slotsString,
                    InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto b = item.getBehaviourByID("HealingItemBehaviour");
        if (b != nullptr) {
            float healing = dynamic_cast<HealingItemBehaviour &>(*b).healingAmount;
            font.drawText("${white}$[red]+${black}$[white] Can be used to heal for " + std::to_string(healing),
                    InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto prop = item.getProperty("WaterContainer");
        if (prop != nullptr) {
            auto waterContainer = boost::any_cast<WaterContainerProperty::WaterContainer>(prop->getValue());
            int current = waterContainer.getAmount();
            font.drawText("It can hold $[cyan]" + std::to_string(waterContainer.getMaxCapacity()) + "$[white] drams of water." +
                          (current > 0 ? " It currently holds $[cyan]" + std::to_string(current) +
                                         "$[white] drams of water." : ""),
                    InventoryScreen::X_OFFSET, yOffset + y++);
        }
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
    if (item->hasProperty("Eatable"))
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
        int weight = boost::any_cast<int>(item->getProperty("Pickuppable")->getValue());

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

void LootingDialog::setViewingDescription(bool viewingDescription) {
    mViewingDescription = viewingDescription;
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


void InspectionDialog:: handleInput(SDL_KeyboardEvent &e) {
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

PlayerEntity &InspectionDialog::getPlayer() {
    return mPlayer;
}

const Point &InspectionDialog::getChosenPoint() const {
    return mChosenPoint;
}

void InspectionDialog::setChosenPoint(const Point &chosenPoint) {
    mChosenPoint = chosenPoint;
}

bool InspectionDialog::isThereAnEntity() const {
    return mThereIsAnEntity;
}

void InspectionDialog::setViewingDescription(bool viewingDescription) {
    mViewingDescription = viewingDescription;
}

bool InspectionDialog::isSelectingFromMultipleOptions() const {
    return mSelectingFromMultipleOptions;
}

void InspectionDialog::setChosenIndex(int chosenIndex) {
    mChosenIndex = chosenIndex;
}

void CraftingScreen::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }

    if (mChoosingPositionInWorld)
        mShouldRenderWorld = true;
    else
        mShouldRenderWorld = false;
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
        if (!a->hasProperty("CraftingMaterial"))
            return false;
        if (std::find(mCurrentlyChosenMaterials.begin(), mCurrentlyChosenMaterials.end(), a->mID) != mCurrentlyChosenMaterials.end())
            return false;

        auto type = boost::any_cast<CraftingMaterialProperty::Data>(a->getProperty("CraftingMaterial")->getValue()).type;
        return rm.mRecipes[mChosenRecipe]->mIngredients[mChosenIngredient].mEntityType == type;
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
    mShouldRenderWorld = false;
    mState = std::make_unique<ChoosingRecipeCraftingScreenState>();
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

int CraftingScreen::getChosenRecipe() const {
    return mChosenRecipe;
}

void CraftingScreen::setChosenRecipe(int chosenRecipe) {
    mChosenRecipe = chosenRecipe;
}

int CraftingScreen::getChosenIngredient() const {
    return mChosenIngredient;
}

void CraftingScreen::setChosenIngredient(int chosenIngredient) {
    mChosenIngredient = chosenIngredient;
}

void CraftingScreen::setChosenMaterial(int chosenMaterial) {
    mChosenMaterial = chosenMaterial;
}

void CraftingScreen::setLayer(CraftingScreen::CraftingLayer layer) {
    mLayer = layer;
}

void CraftingScreen::setChoosingPositionInWorld(bool choosingPositionInWorld) {
    mChoosingPositionInWorld = choosingPositionInWorld;
}

bool CraftingScreen::isHaveChosenPositionInWorld() const {
    return mHaveChosenPositionInWorld;
}

void CraftingScreen::setCouldNotBuildAtPosition(bool couldNotBuildAtPosition) {
    mCouldNotBuildAtPosition = couldNotBuildAtPosition;
}

PlayerEntity &CraftingScreen::getPlayer() const {
    return mPlayer;
}

std::vector<std::string> & CraftingScreen::getCurrentlyChosenMaterials() {
    return mCurrentlyChosenMaterials;
}

void CraftingScreen::setCurrentRecipe(std::unique_ptr<Recipe> currentRecipe) {
    mCurrentRecipe = std::move(currentRecipe);
}

std::unique_ptr<Recipe> &CraftingScreen::getCurrentRecipe() {
    return mCurrentRecipe;
}

void EquipmentScreen::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
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
                auto b = entity->getProperty("MeleeWeaponDamage");
                if (b != nullptr) {
                    lines[i] += " $[red]$(heart)$[white]" + std::to_string(mPlayer.mHitTimes)
                                + "d" +
                                std::to_string(mPlayer.mHitAmount + boost::any_cast<int>(b->getValue()));
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

EquipmentSlot EquipmentScreen::getChosenSlot() const {
    return mChosenSlot;
}

void EquipmentScreen::setChosenSlot(EquipmentSlot chosenSlot) {
    mChosenSlot = chosenSlot;
}

PlayerEntity &EquipmentScreen::getPlayer() {
    return mPlayer;
}

void EquipmentScreen::setChoosingEquipmentAction(bool choosingEquipmentAction) {
    mChoosingEquipmentAction = choosingEquipmentAction;
}

void EquipmentScreen::setChoosingEquipmentActionIndex(int choosingEquipmentActionIndex) {
    mChoosingEquipmentActionIndex = choosingEquipmentActionIndex;
}

void EquipmentScreen::setChoosingNewEquipment(bool choosingNewEquipment) {
    mChoosingNewEquipment = choosingNewEquipment;
}

void EquipmentScreen::setChoosingNewEquipmentIndex(int choosingNewEquipmentIndex) {
    mChoosingNewEquipmentIndex = choosingNewEquipmentIndex;
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
        disable();
}

void HelpScreen::render(Font &font) {
    const int yPadding = 2;
    const int xPadding = 2;
    int y = 0;

    for (auto line : cDisplayLines) {
        font.drawText(line, xPadding, yPadding + y++);
    }
}
