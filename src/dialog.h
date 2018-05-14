
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include "font.h"
#include "entities.h"

void showMessageBox(Font& font, const std::vector<std::string> &contents, int padding, int x, int y);
inline void showMessageBox(Font& font, const std::vector<std::string> &contents, int x, int y) {
    showMessageBox(font, contents, 1, x, y);
}
inline void showMessageBox(Font& font, const std::string &message, int x, int y) {
    showMessageBox(font, std::vector<std::string>{message}, 1, x, y);
}

//void inventoryScreen(Font& font, Entity &player, SDL_KeyboardEvent &e, bool &inventoryScreen);

class PlayerEntity;

class InventoryScreen {
public:
    static const int X_OFFSET = 4;
    static const int X_STATUS_OFFSET = 10;
    static const int Y_OFFSET = 4;
    static const int WORD_WRAP_COLUMN = 60;

    PlayerEntity &player;
    int chosenIndex {0};
    bool enabled {false};
    bool viewingDescription {false};

    explicit InventoryScreen(PlayerEntity &player) : player(player) {}

    void handleInput(SDL_KeyboardEvent &e);
    void render(Font& font);
};

class LootingDialog {
public:
    const int DIALOG_WIDTH = 30;

    bool enabled {false};
    bool viewingDescription {false};
    bool showingTooMuchWeightMessage {false};
    PlayerEntity &player;
    std::vector<std::shared_ptr<Entity>> itemsToShow;
    int chosenIndex {0};
    std::shared_ptr<Entity> entityToTransferFrom;

    explicit LootingDialog(PlayerEntity &player) : player(player) {}

    void showItemsToLoot(std::vector<std::shared_ptr<Entity>> items);
    void showItemsToLoot(std::vector<std::shared_ptr<Entity>> items, std::shared_ptr<Entity> entityToTransferFrom);

    void handleInput(SDL_KeyboardEvent &e);
    void render(Font& font);
};

class InspectionDialog {
    inline Point clipToScreenEdge(const Point &p) const;
public:
    bool enabled {false};
    PlayerEntity &player;
    Point chosenPoint;
    bool selectingFromMultipleOptions {false};
    int chosenIndex {0};
    bool viewingDescription {false};
    bool thereIsAnEntity {false};

    explicit InspectionDialog(PlayerEntity &player) : player(player) {}

    void handleInput(SDL_KeyboardEvent &e);
    void render(Font& font);
};

#endif // DIALOG_H_