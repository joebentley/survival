
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include <ctime>
#include "font.h"
#include "entities.h"
#include "recipe.h"

enum class ScreenType {
    NOTIFICATION,
    INVENTORY,
    LOOTING,
    INSPECTION,
    CRAFTING,
    EQUIPMENT,
    HELP
};

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
        std::vector<std::string> mContents;
        int mPadding;
        int mX;
        int mY;
    };

    std::deque<MessageBoxData> mRenderingQueue;
};

struct NotificationMessageRenderer {
    static NotificationMessageRenderer& getInstance() {
        static NotificationMessageRenderer instance;
        return instance;
    }

    NotificationMessageRenderer() {
        previousTime = clock();
    }

    NotificationMessageRenderer(const NotificationMessageRenderer&) = delete;
    void operator=(const NotificationMessageRenderer&) = delete;

    void queueMessage(const std::string &message);
    void render(Font &font);

    std::deque<std::string> getMessages() const {
        return mAllMessages;
    }

private:
    const int cMaxOnScreen {6};
    const int cInitialYPos {SCREEN_HEIGHT - 2};
    const float cAlphaDecayPerSec {1};
    std::deque<std::string> mMessagesToBeRendered;
    std::deque<std::string> mAllMessages;
    float mAlpha {1};
    clock_t previousTime {0};
};

struct Screen {
    explicit Screen(bool shouldRenderWorld) : mShouldRenderWorld(shouldRenderWorld) {}

    virtual void enable() {
        mEnabled = true;
    }
    virtual void disable() {
        mEnabled = false;
    }
    bool isEnabled() const {
        return mEnabled;
    }
    bool shouldRenderWorld() const {
        return mShouldRenderWorld;
    }
    virtual void handleInput(SDL_KeyboardEvent &e) = 0;
    virtual void render(Font &font) = 0;

protected:
    bool mEnabled {false};
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

    explicit InventoryScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void enable() override {
        mChosenIndex = 0;
        Screen::enable();
    }

private:
    PlayerEntity &mPlayer;
    int mChosenIndex {0};
    bool mViewingDescription {false};
};

struct LootingDialog : Screen {
    explicit LootingDialog(PlayerEntity &player) : Screen(true), mPlayer(player) {}

    void showItemsToLoot(std::vector<Entity *> items);
    void showItemsToLoot(std::vector<Entity *> items, Entity *entityToTransferFrom);

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;
private:
    const int DIALOG_WIDTH = 30;

    bool mViewingDescription {false};
    bool mShowingTooMuchWeightMessage {false};
    PlayerEntity &mPlayer;
    std::vector<Entity *> mItemsToShow;
    int mChosenIndex {0};
    Entity *mEntityToTransferFrom;
};

struct InspectionDialog : Screen {
    explicit InspectionDialog(PlayerEntity &player) : Screen(true), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void enableAtPoint(Point initialPoint) {
        mChosenPoint = initialPoint;
        Screen::enable();
    }

private:
    inline Point clipToScreenEdge(const Point &p) const;
    PlayerEntity &mPlayer;
    Point mChosenPoint;
    bool mSelectingFromMultipleOptions {false};
    int mChosenIndex {0};
    bool mViewingDescription {false};
    bool mThereIsAnEntity {false};
};

struct Recipe;
struct CraftingScreen : Screen {
    explicit CraftingScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

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
    int mChosenRecipe {0};
    int mChosenIngredient {0};
    int mChosenMaterial {0};
    PlayerEntity &mPlayer;
    CraftingLayer mLayer {CraftingLayer::RECIPE};
    std::unique_ptr<Recipe> mCurrentRecipe {nullptr};

    bool mChoosingPositionInWorld {false};
    bool mHaveChosenPositionInWorld {false};
    bool mCouldNotBuildAtPosition {false};

    std::vector<std::string> mCurrentlyChosenMaterials;
    /// Filter inventory items for items that are of the currently chosen material type and are not in currentlyChosenMaterials
    /// \return vector of shared pointers to the inventory items as described above
    std::vector<Entity *> filterInventoryForChosenMaterials();
    bool currentRecipeSatisfied();
    void tryToBuildAtPosition(Point posOffset);
    void buildItem(Point pos);
};

struct EquipmentScreen : Screen {
    explicit EquipmentScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;

    /// Render equipment screen
    /// \param font Font object to render using
    void render(Font &font) override;

    void reset();
    void enable() override;

private:
    PlayerEntity &mPlayer;
    EquipmentSlot mChosenSlot {EquipmentSlot::HEAD};
    bool mChoosingEquipmentAction {false};
    int mChoosingEquipmentActionIndex {0};
    bool mChoosingNewEquipment {false};
    int mChoosingNewEquipmentIndex {0};
};

struct HelpScreen : Screen {
    HelpScreen() : Screen(false) {}

    const std::vector<std::string> cDisplayLines = {
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