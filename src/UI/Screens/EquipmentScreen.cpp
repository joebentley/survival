#include "EquipmentScreen.h"

#include "../../Color.h"
#include "../../Entity/EntityManager.h"
#include "../../Entity/PlayerEntity.h"
#include "../../Font.h"
#include "../MessageBoxRenderer.h"

void EquipmentScreen::handleInput(SDL_KeyboardEvent &e) {
    // TODO: this repeated code should be abstracted out
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
        Color bColor = Color::getColor("black");
        if (mChosenSlot == slot)
            bColor = Color::getColor("blue");

        std::string currentlyEquipped = mPlayer.getEquipmentID(slot);
        if (!currentlyEquipped.empty()) {
            auto e = EntityManager::getInstance().getEntityByID(currentlyEquipped);
            font.drawText(e->mGraphic + " " + e->mName +
                              (slot == EquipmentSlot::RIGHT_HAND
                                   ? " $[red]$(heart)$[white]" + std::to_string(mPlayer.mHitTimes) + "d" +
                                         std::to_string(mPlayer.computeMaxDamage())
                                   : ""),
                          20, y);
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

        for (std::vector<std::string>::size_type i = 0; i < equippableIDs.size(); ++i) {
            auto ID = equippableIDs[i];
            auto entity = EntityManager::getInstance().getEntityByID(ID);

            lines.emplace_back((i == (size_t)mChoosingNewEquipmentIndex ? "$(right)" : " ") + entity->mGraphic + " " +
                               entity->mName);

            if (mChosenSlot == EquipmentSlot::RIGHT_HAND) {
                auto b = entity->getProperty("MeleeWeaponDamage");
                if (b != nullptr) {
                    lines[i] += " $[red]$(heart)$[white]" + std::to_string(mPlayer.mHitTimes) + "d" +
                                std::to_string(mPlayer.mHitAmount + std::any_cast<int>(b->getValue()));
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

EquipmentSlot EquipmentScreen::getChosenSlot() const { return mChosenSlot; }

void EquipmentScreen::setChosenSlot(EquipmentSlot chosenSlot) { mChosenSlot = chosenSlot; }

PlayerEntity &EquipmentScreen::getPlayer() { return mPlayer; }

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
