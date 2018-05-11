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
        auto item = player.inventory[chosenIndex];

        font.drawText(item->graphic + " " + item->name, X_OFFSET, Y_OFFSET);
        font.drawText(item->shortDesc, X_OFFSET, Y_OFFSET + 2);

        auto words = wordWrap(item->longDesc, WORD_WRAP_COLUMN);
        for (int i = 0; i < words.size(); ++i) {
            font.drawText(words[i], X_OFFSET, Y_OFFSET + 4 + i);
        }

        font.drawText("esc-return to inv", 1, SCREEN_HEIGHT - 2);

        return;
    }

    font.draw("right", X_OFFSET - 1, chosenIndex + Y_OFFSET);

    for (int i = 0; i < player.inventory.size(); ++i) {
        auto item = player.inventory[i];

        font.drawText(item->graphic + " " + item->name, X_OFFSET, i + Y_OFFSET);
    }

    font.drawText("e-eat  d-drop  return-view desc  esc-exit inv", 1, SCREEN_HEIGHT - 2);
}
