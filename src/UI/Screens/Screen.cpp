#include "Screen.h"
#include "../../Behaviour/Item/HealingItemBehaviour.h"
#include "../../Entity/Entity.h"
#include "../../Font.h"
#include "../../Property/Properties/AdditionalCarryWeightProperty.h"
#include "../../Property/Properties/EquippableProperty.h"
#include "../../Property/Properties/MeleeWeaponDamageProperty.h"
#include "../../Property/Properties/PickuppableProperty.h"
#include "../../Property/Properties/WaterContainerProperty.h"
#include "../../World.h"
#include "../../utils.h"
#include "InventoryScreen.h"

void drawDescriptionScreen(Font &font, Entity &item) {
    font.drawText(item.mGraphic + " " + item.mName, InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET);
    font.drawText(item.mShortDesc, InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 2);

    auto words = wordWrap(item.mLongDesc, InventoryScreen::WORD_WRAP_COLUMN);
    for (std::vector<std::string>::size_type i = 0; i < words.size(); ++i) {
        font.drawText(words[i], InventoryScreen::X_OFFSET, InventoryScreen::Y_OFFSET + 4 + (int)i);
    }

    int y = 2;
    const int yOffset = InventoryScreen::Y_OFFSET + 4 + static_cast<int>(words.size());

    {
        auto b = item.getProperty<PickuppableProperty>();
        if (b != nullptr) {
            int weight = b->weight;
            font.drawText("It weighs " + std::to_string(weight) + (weight == 1 ? " pound" : " pounds"),
                          InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto b = item.getProperty<MeleeWeaponDamageProperty>();
        if (b != nullptr) {
            int damage = b->damage;
            font.drawText("It adds $[red]" + std::to_string(damage) + "$[white] to your damage roll",
                          InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto b = item.getProperty<AdditionalCarryWeightProperty>();
        if (b != nullptr) {
            int carry = b->additionalCarryWeight;
            font.drawText("It adds an extra " + std::to_string(carry) + "lb to your max carry weight",
                          InventoryScreen::X_OFFSET, yOffset + y++);
        }
    }

    {
        auto equippable = item.getProperty<EquippableProperty>();
        if (equippable != nullptr) {
            const auto &slots = equippable->getEquippableSlots();
            std::string slotsString;

            for (auto slot = slots.cbegin(); slot != slots.cend(); ++slot) {
                slotsString += slotToString(*slot);
                if (slot != slots.cend() - 1)
                    slotsString += ", ";
            }

            font.drawText("Can be equipped in: " + slotsString, InventoryScreen::X_OFFSET, yOffset + y++);
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
        auto waterContainer = item.getProperty<WaterContainerProperty>();
        if (waterContainer != nullptr) {
            int current = waterContainer->getAmount();
            font.drawText(
                "It can hold $[cyan]" + std::to_string(waterContainer->getMaxCapacity()) + "$[white] drams of water." +
                    (current > 0 ? " It currently holds $[cyan]" + std::to_string(current) + "$[white] drams of water."
                                 : ""),
                InventoryScreen::X_OFFSET, yOffset + y + 1);
        }
    }

    font.drawText("esc-back", 1, World::SCREEN_HEIGHT - 2);
}
