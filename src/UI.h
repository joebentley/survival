
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include "font.h"
#include "entities.h"
#include "recipe.h"


void showMessageBox(Font& font, const std::vector<std::string> &contents, int padding, int x, int y);

struct MessageBoxRenderer {
    static MessageBoxRenderer& getInstance() {
        static MessageBoxRenderer instance;
        return instance;
    }

    MessageBoxRenderer() = default;
    MessageBoxRenderer(const MessageBoxRenderer&) = delete;
    void operator=(const MessageBoxRenderer&) = delete;

    void queueMessageBox(const std::vector<std::string> &contents, int padding, int x, int y);
    inline void queueMessageBox(const std::vector<std::string> &contents, int x, int y) {
        queueMessageBox(contents, 1, x, y);
    }
    inline void queueMessageBox(const std::string &message, int x, int y) {
        queueMessageBox(std::vector<std::string>{message}, 1, x, y);
    }
    void queueMessageBoxCentered(const std::vector<std::string> &contents, int padding);
    inline void queueMessageBoxCentered(const std::string &message, int padding) {
        queueMessageBoxCentered(std::vector<std::string>{message}, padding);
    }
    void render(Font &font);

private:
    struct MessageBoxData {
        std::vector<std::string> contents;
        int padding;
        int x;
        int y;
    };

    std::deque<MessageBoxData> renderingQueue;
};

struct NotificationMessageRenderer {
    static NotificationMessageRenderer& getInstance() {
        static NotificationMessageRenderer instance;
        return instance;
    }

    NotificationMessageRenderer() = default;
    NotificationMessageRenderer(const NotificationMessageRenderer&) = delete;
    void operator=(const NotificationMessageRenderer&) = delete;

    void queueMessage(const std::string &message);
    void render(Font &font);

    std::deque<std::string> getMessages() const {
        return allMessages;
    }

private:
    const int MAX_ON_SCREEN {6};
    const int INITIAL_Y_POS {SCREEN_HEIGHT - 2};
    const int ALPHA_DECAY_PER_RENDER {2};
    std::deque<std::string> messagesToBeRendered;
    std::deque<std::string> allMessages;
    int alpha {0xFF};
};

struct Screen {
    Screen(bool shouldRenderWorld) : mShouldRenderWorld(shouldRenderWorld) {}

    virtual void enable() {
        enabled = true;
    }
    virtual void disable() {
        enabled = false;
    }
    bool isEnabled() const {
        return enabled;
    }
    bool shouldRenderWorld() const {
        return mShouldRenderWorld;
    }
    virtual void handleInput(SDL_KeyboardEvent &e) = 0;
    virtual void render(Font &font) = 0;

protected:
    bool enabled {false};
    bool mShouldRenderWorld;
};

struct NotificationMessageScreen : Screen {
    NotificationMessageScreen() : Screen(false) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;
};

struct PlayerEntity;

struct InventoryScreen : Screen {
    static const int X_OFFSET = 4;
    static const int X_STATUS_OFFSET = 10;
    static const int Y_OFFSET = 4;
    static const int WORD_WRAP_COLUMN = 60;

    explicit InventoryScreen(PlayerEntity &player) : Screen(false), player(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void enable() override {
        chosenIndex = 0;
        Screen::enable();
    }

private:
    PlayerEntity &player;
    int chosenIndex {0};
    bool viewingDescription {false};
};

struct LootingDialog : Screen {
    explicit LootingDialog(PlayerEntity &player) : Screen(true), player(player) {}

    void showItemsToLoot(std::vector<std::shared_ptr<Entity>> items);
    void showItemsToLoot(std::vector<std::shared_ptr<Entity>> items, std::shared_ptr<Entity> entityToTransferFrom);

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;
private:
    const int DIALOG_WIDTH = 30;

    bool viewingDescription {false};
    bool showingTooMuchWeightMessage {false};
    PlayerEntity &player;
    std::vector<std::shared_ptr<Entity>> itemsToShow;
    int chosenIndex {0};
    std::shared_ptr<Entity> entityToTransferFrom;
};

struct InspectionDialog : Screen {
    explicit InspectionDialog(PlayerEntity &player) : Screen(true), player(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void enableAtPoint(Point initialPoint) {
        chosenPoint = initialPoint;
        Screen::enable();
    }

private:
    inline Point clipToScreenEdge(const Point &p) const;
    PlayerEntity &player;
    Point chosenPoint;
    bool selectingFromMultipleOptions {false};
    int chosenIndex {0};
    bool viewingDescription {false};
    bool thereIsAnEntity {false};
};

struct Recipe;
struct CraftingScreen : Screen {
    explicit CraftingScreen(PlayerEntity &player) : Screen(false), player(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;

    /// Render crafting screen
    /// \param font Font object to render using
    void render(Font &font) override;
    void reset();

    void enable() override;

    enum class CraftingLayer {
        RECIPE,
        INGREDIENT,
        MATERIAL
    };
private:
    int chosenRecipe {0};
    int chosenIngredient {0};
    int chosenMaterial {0};
    PlayerEntity &player;
    CraftingLayer layer {CraftingLayer::RECIPE};
    std::unique_ptr<Recipe> currentRecipe {nullptr};

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

struct EquipmentScreen : Screen {
    explicit EquipmentScreen(PlayerEntity &player) : Screen(false), player(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;

    /// Render equipment screen
    /// \param font Font object to render using
    void render(Font &font) override;

    void reset();
    void enable() override;

private:
    PlayerEntity &player;
    EquipmentSlot chosenSlot {EquipmentSlot::HEAD};
    bool choosingEquipmentAction {false};
    int choosingEquipmentActionIndex {0};
    bool choosingNewEquipment {false};
    int choosingNewEquipmentIndex {0};
};

struct HelpScreen : Screen {
    HelpScreen() : Screen(false) {}

    const std::vector<std::string> displayLines = {
        "Diagonal movement: y u b n",
        "Cardinal movement: h j k l",
        "Get item: g",
        "Wait 1 tick: period (.)",
        "Force attack: shift + direction",
        "Inventory: i",
        "Equipment: e",
        "Crafting: c",
        "Message log: m",
        "Inspect: semicolon (;)",
        "This screen: ?"
    };

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;
};
#endif // DIALOG_H_