#include "InventoryScreen.h"

#include "../../Entity/PlayerEntity.h"
#include "../../Font.h"
#include "../../World.h"

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

    for (size_t i = 0; i < mPlayer.getInventorySize(); ++i) {
        auto item = mPlayer.getInventoryItem((int)i);
        std::string displayString = item->mGraphic + " " + item->mName;

        font.drawText(displayString, X_OFFSET, (int)i + Y_OFFSET);
        if (mPlayer.hasEquipped(item->mID))
            font.drawText("(" + slotToString(mPlayer.getEquipmentSlotByID(item->mID)) + ")",
                          X_OFFSET + Font::getFontStringLength(displayString) + 2, (int)i + Y_OFFSET);
    }

    std::string colorStr;
    float hpPercent = mPlayer.mHp / mPlayer.mMaxHp;

    if (hpPercent > 0.7)
        colorStr = "$[green]";
    else if (hpPercent > 0.3)
        colorStr = "$[yellow]";
    else
        colorStr = "$[red]";

    font.drawText("${black}" + colorStr + "hp " + std::to_string((int)ceil(mPlayer.mHp)) + "/" +
                      std::to_string((int)ceil(mPlayer.mMaxHp)),
                  World::SCREEN_WIDTH - X_STATUS_OFFSET, 1);

    if (mPlayer.hunger > 0.7)
        font.drawText("${black}$[green]sated", World::SCREEN_WIDTH - X_STATUS_OFFSET, 2);
    else if (mPlayer.hunger > 0.3)
        font.drawText("${black}$[yellow]hungry", World::SCREEN_WIDTH - X_STATUS_OFFSET, 2);
    else
        font.drawText("${black}$[red]starving", World::SCREEN_WIDTH - X_STATUS_OFFSET, 2);

    font.drawText("${black}" + std::to_string(mPlayer.getCarryingWeight()) + "/" +
                      std::to_string(mPlayer.getMaxCarryWeight()) + "lb",
                  World::SCREEN_WIDTH - X_STATUS_OFFSET, 4);

    std::string helpString;
    auto item = mPlayer.getInventoryItem(mChosenIndex);
    if (item->hasProperty("Eatable"))
        helpString += "e-eat  ";
    if (item->hasBehaviour("ApplyableBehaviour"))
        helpString += "a-apply  ";
    font.drawText(helpString + "d-drop  return-view desc  esc-exit inv", 1, World::SCREEN_HEIGHT - 2);
}

PlayerEntity &InventoryScreen::getPlayer() const { return mPlayer; }

int InventoryScreen::getChosenIndex() const { return mChosenIndex; }

void InventoryScreen::setChosenIndex(int chosenIndex) { mChosenIndex = chosenIndex; }

void InventoryScreen::setViewingDescription(bool viewingDescription) { mViewingDescription = viewingDescription; }
