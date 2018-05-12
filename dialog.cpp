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

void showMessageBox(Font& font, const std::string& message, int x, int y) {
    int numChars = getFontStringLength(message);

    font.drawText("${black}$(p23)" + repeat(numChars + 4, "$(p27)") + "$(p9)", x, y);
    font.drawText("${black}$(p8)" + std::string(numChars + 4, ' ') + "$(p8)", x, y+1);
    font.drawText("${black}$(p8)  " + message + "${black}  $[white]$(p8)", x, y+2);
    font.drawText("${black}$(p8)" + std::string(numChars + 4, ' ') + "$(p8)", x, y+3);
    font.drawText("${black}$(p22)" + repeat(numChars + 4, "$(p27)") + "$(p10)", x, y+4);
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
            if (!viewingDescription && !player.inventory.empty()) {
                if (chosenIndex < (player.inventory.size() - 1))
                    chosenIndex++;
                else
                    chosenIndex = 0;
            }
            break;
        case SDLK_k:
            if (!viewingDescription && !player.inventory.empty()) {
                if (chosenIndex > 0)
                    chosenIndex--;
                else
                    chosenIndex = (int) player.inventory.size() - 1;
            }
            break;
        case SDLK_d:
            if (!viewingDescription && !player.inventory.empty()) {
                player.dropItem(chosenIndex);
                chosenIndex = 0;
            }
            break;
        case SDLK_e:
            if (!viewingDescription && !player.inventory.empty()) {
                auto item = player.inventory[chosenIndex];
                if (item->hasBehaviour("EatableBehaviour")) {
                    player.hunger = std::min(player.hunger + dynamic_cast<EatableBehaviour&>(*(*item).getBehaviourByID("EatableBehaviour")).hungerRestoration, 1.0);
                }
                player.inventory.erase(player.inventory.begin() + chosenIndex);
                item->destroy();
                chosenIndex = 0;
            }
            break;
        case SDLK_RETURN:
            if (!player.inventory.empty())
                viewingDescription = true;
            break;
    }
}

void InventoryScreen::render(Font &font) {
    if (viewingDescription) {
        drawDescriptionScreen(font, *player.inventory[chosenIndex]);
        return;
    }

    font.draw("right", X_OFFSET - 1, chosenIndex + Y_OFFSET);

    for (int i = 0; i < player.inventory.size(); ++i) {
        auto item = player.inventory[i];

        font.drawText(item->graphic + " " + item->name, X_OFFSET, i + Y_OFFSET);
    }

    std::string colorStr;
    double hpPercent = player.hp / player.maxhp;

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

    font.drawText("e-eat  d-drop  return-view desc  esc-exit inv", 1, SCREEN_HEIGHT - 2);
}

void LootingDialog::showItemsToLoot(std::vector<std::shared_ptr<Entity>> items) {
    itemsToShow = std::move(items);
    enabled = true;
}

void LootingDialog::handleInput(SDL_KeyboardEvent &e) {
    switch (e.keysym.sym) {
        case SDLK_ESCAPE:
            if (viewingDescription)
                viewingDescription = false;
            else if (showingTooMuchWeightMessage)
                showingTooMuchWeightMessage = false;
            else
                enabled = false;
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
