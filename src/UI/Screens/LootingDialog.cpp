#include "LootingDialog.h"

#include "../../Entity/Entity.h"
#include "../../Font.h"
#include "../../Property/Properties/PickuppableProperty.h"
#include "../../World.h"
#include "../../utils.h"
#include "../MessageBoxRenderer.h"

void LootingDialog::showItemsToLoot(std::vector<Entity *> items) {
    mItemsToShow = std::move(items);
    mEnabled = true;
}

void LootingDialog::showItemsToLoot(std::vector<Entity *> items, Entity *entityToTransferFrom) {
    mItemsToShow = std::move(items);
    mEntityToTransferFrom = entityToTransferFrom;
    mEnabled = true;
}

void LootingDialog::handleInput(SDL_KeyboardEvent &e) {
    auto newState = mState->handleInput(*this, e);
    if (newState != nullptr) {
        mState->onExit(*this);
        mState = std::move(newState);
        mState->onEntry(*this);
    }

    mShouldRenderWorld = !mViewingDescription;
}

void LootingDialog::render(Font &font) {
    if (mShowingTooMuchWeightMessage) {
        const std::string &displayString = "You cannot carry that much!";
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

    const int x = World::SCREEN_WIDTH / 2 - (DIALOG_WIDTH + 4) / 2;
    const int y = 10;

    font.drawText("${black}$(p23)" + repeat(DIALOG_WIDTH + 4, "$(p27)") + "$(p9)", x, y);
    font.drawText("${black}$(p8)" + std::string(DIALOG_WIDTH + 4, ' ') + "$(p8)", x, y + 1);

    for (int i = 0; i < numItems; ++i) {
        auto item = mItemsToShow[i];
        const int weight = item->getProperty<PickuppableProperty>()->weight;

        std::string weightString = std::to_string(weight);
        std::string string = item->mGraphic + " " + item->mName.substr(0, DIALOG_WIDTH - 6);
        string += std::string(DIALOG_WIDTH - Font::getFontStringLength(string) - 3 - weightString.size() + 1, ' ') +
                  "$[white]" + weightString + " lb";

        font.drawText("${black}$(p8)  " + string + "${black} $[white]$(p8)", x, y + 2 + i);
    }

    font.draw("right", x + 2, y + 2 + mChosenIndex);

    font.drawText("${black}$(p8)" + std::string(DIALOG_WIDTH + 4, ' ') + "$(p8)", x, y + numItems + 2);
    std::string string = "g-loot  esc-quit  return-desc";
    font.drawText("${black}$(p8)  " + string + std::string(DIALOG_WIDTH - string.size() + 2, ' ') + "$(p8)", x,
                  y + numItems + 3);
    font.drawText("${black}$(p22)" + repeat(DIALOG_WIDTH + 4, "$(p27)") + "$(p10)", x, y + numItems + 4);
}

void LootingDialog::setViewingDescription(bool viewingDescription) { mViewingDescription = viewingDescription; }

void LootingDialog::setShowingTooMuchWeightMessage(bool showingTooMuchWeightMessage) {
    mShowingTooMuchWeightMessage = showingTooMuchWeightMessage;
}

int LootingDialog::getChosenIndex() const { return mChosenIndex; }

void LootingDialog::setChosenIndex(int chosenIndex) { mChosenIndex = chosenIndex; }

PlayerEntity &LootingDialog::getPlayer() const { return mPlayer; }

std::vector<Entity *> &LootingDialog::getItemsToShow() { return mItemsToShow; }

Entity *LootingDialog::getEntityToTransferFrom() const { return mEntityToTransferFrom; }
