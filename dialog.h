
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include "font.h"
#include "entities.h"

void showMessageBox(Font& font, const std::string &message, int x, int y);

//void inventoryScreen(Font& font, Entity &player, SDL_KeyboardEvent &e, bool &inventoryScreen);

class PlayerEntity;

class InventoryScreen {
public:
    const int Y_OFFSET = 4;
    const int WORD_WRAP_COLUMN = 60;

    PlayerEntity &player;
    int chosenIndex {0};
    bool enabled {false};
    bool viewingDescription {false};

    explicit InventoryScreen(PlayerEntity &player) : player(player) {}

    void handleInput(SDL_KeyboardEvent &e);
    void render(Font& font);
};

#endif // DIALOG_H_