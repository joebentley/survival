
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include "font.h"
#include "entities.h"
#include "recipe.h"

void showMessageBox(Font& font, const std::vector<std::string> &contents, int padding, int x, int y);
inline void showMessageBox(Font& font, const std::vector<std::string> &contents, int x, int y) {
    showMessageBox(font, contents, 1, x, y);
}
inline void showMessageBox(Font& font, const std::string &message, int x, int y) {
    showMessageBox(font, std::vector<std::string>{message}, 1, x, y);
}
void showMessageBoxCentered(Font& font, const std::vector<std::string> &contents, int padding);
inline void showMessageBoxCentered(Font& font, const std::string &message, int padding) {
    showMessageBoxCentered(font, std::vector<std::string>{message}, padding);
}

struct PlayerEntity;

struct InventoryScreen {
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

struct LootingDialog {
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

struct Recipe;
struct CraftingScreen {
    explicit CraftingScreen(PlayerEntity &player) : player(player) {}

    bool enabled {false};

    void handleInput(SDL_KeyboardEvent &e);

    /// Render crafting screen
    /// \param font Font object to render using
    /// \param world World object used for rendering
    /// \param lightMapTexture Light map texture used for rendering
    void render(Font &font, World &world, LightMapTexture &lightMapTexture);
    void reset();

    void enable();
private:
    enum class CraftingLayer {
        RECIPE,
        INGREDIENT,
        MATERIAL
    };

    const int SHOW_CREATED_DISPLAY_LENGTH = 2000;

    int chosenRecipe {0};
    int chosenIngredient {0};
    int chosenMaterial {0};
    PlayerEntity &player;
    CraftingLayer layer {CraftingLayer::RECIPE};
    std::unique_ptr<Recipe> currentRecipe {nullptr};

    std::string createdMessage;
    int createdMessageTimer {0};

    bool choosingPositionInWorld {false};
    bool haveChosenPositionInWorld {false};
    bool couldNotBuildAtPosition {false};

    std::vector<std::string> currentlyChosenMaterials;
    /// Filter inventory items for items that are of the currently chosen material type and are not in currentlyChosenMaterials
    /// \return vector of shared pointers to the inventory items as described above
    std::vector<std::shared_ptr<Entity>> filterInventoryForChosenMaterials();
    bool currentRecipeSatisfied();
    void tryToBuildAtPosition(Point posOffset);
    void buildItem(Point pos);
};

struct EquipmentScreen {
    explicit EquipmentScreen(PlayerEntity &player) : player(player) {}

    bool enabled {false};

    void handleInput(SDL_KeyboardEvent &e);

    /// Render equipment screen
    /// \param font Font object to render using
    void render(Font &font);

    void reset();
    void enable();

private:
    PlayerEntity &player;
    EquipmentSlot chosenSlot {EquipmentSlot::HEAD};
    bool choosingEquipmentAction {false};
    int choosingEquipmentActionIndex {0};
    bool choosingNewEquipment {false};
    int choosingNewEquipmentIndex {0};
};

#endif // DIALOG_H_