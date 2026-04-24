#pragma once

#include "Screen.h"

#include <vector>

struct HelpScreen : Screen {
    HelpScreen() : Screen(false) {}

    const std::vector<std::string> cDisplayLines = {"Diagonal movement: y u b n",
                                                    "Cardinal movement: h j k l",
                                                    "Get item: g",
                                                    "Wait 1 tick: period (.)",
                                                    "Force attack: shift + direction",
                                                    "Inventory: i",
                                                    "Equipment: e",
                                                    "Crafting: c",
                                                    "Message log: m",
                                                    "Inspect: semicolon (;)",
                                                    "Interact: space",
                                                    "This screen: ?"};

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;
};
