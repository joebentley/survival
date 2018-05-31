
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include <ctime>
#include "Font.h"
#include "Entities.h"
#include "Recipe.h"
#include "UIState.h"

enum class ScreenType {
    NOTIFICATION,
    INVENTORY,
    LOOTING,
    INSPECTION,
    CRAFTING,
    EQUIPMENT,
    HELP,
    DEBUG
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
    static const int WORD_WRAP_COLUMN = 40;

    explicit InventoryScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void enable() override {
        mChosenIndex = 0;
        Screen::enable();
    }

    PlayerEntity &getPlayer() const;

    int getChosenIndex() const;
    void setChosenIndex(int chosenIndex);

    void setViewingDescription(bool viewingDescription);

private:
    PlayerEntity &mPlayer;
    int mChosenIndex {0};
    bool mViewingDescription {false};
    std::unique_ptr<InventoryScreenState> mState {std::make_unique<ViewingInventoryState>()};
};

struct LootingDialog : Screen {
    explicit LootingDialog(PlayerEntity &player) : Screen(true), mPlayer(player) {}

    void showItemsToLoot(std::vector<Entity *> items);
    void showItemsToLoot(std::vector<Entity *> items, Entity *entityToTransferFrom);

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void setViewingDescription(bool viewingDescription);

    void setShowingTooMuchWeightMessage(bool showingTooMuchWeightMessage);

    int getChosenIndex() const;
    void setChosenIndex(int chosenIndex);

    PlayerEntity &getPlayer() const;
    std::vector<Entity *> & getItemsToShow();
    Entity *getEntityToTransferFrom() const;

private:
    const int DIALOG_WIDTH = 30;

    bool mViewingDescription {false};
    bool mShowingTooMuchWeightMessage {false};
    PlayerEntity &mPlayer;
    std::vector<Entity *> mItemsToShow;
    int mChosenIndex {0};
    Entity *mEntityToTransferFrom {nullptr};

    std::unique_ptr<LootingDialogState> mState {std::make_unique<ViewingLootingDialogState>()};
};

struct InspectionDialog : Screen {
    explicit InspectionDialog(PlayerEntity &player) : Screen(true), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font& font) override;

    void enableAtPoint(Point initialPoint) {
        mChosenPoint = initialPoint;
        Screen::enable();
    }

    PlayerEntity &getPlayer();
    const Point &getChosenPoint() const;
    void setChosenPoint(const Point &chosenPoint);
    bool isThereAnEntity() const;
    void setViewingDescription(bool viewingDescription);

    bool isSelectingFromMultipleOptions() const;

    void setChosenIndex(int chosenIndex);

private:
    PlayerEntity &mPlayer;
    Point mChosenPoint;
    bool mSelectingFromMultipleOptions {false};
    int mChosenIndex {0};
    bool mViewingDescription {false};
    bool mThereIsAnEntity {false};

    std::unique_ptr<InspectionDialogState> mState {std::make_unique<ChoosingPositionInspectionDialogState>()};
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

    int getChosenRecipe() const;
    void setChosenRecipe(int chosenRecipe);
    int getChosenIngredient() const;
    void setChosenIngredient(int chosenIngredient);
    void setChosenMaterial(int chosenMaterial);
    void setLayer(CraftingLayer layer);
    void setChoosingPositionInWorld(bool choosingPositionInWorld);
    bool isHaveChosenPositionInWorld() const;
    void setCouldNotBuildAtPosition(bool couldNotBuildAtPosition);

    PlayerEntity &getPlayer() const;
    std::vector<std::string> & getCurrentlyChosenMaterials();

    void setCurrentRecipe(std::unique_ptr<Recipe> currentRecipe);
    std::unique_ptr<Recipe> &getCurrentRecipe();

    /// Filter inventory items for items that are of the currently chosen material type and are not in currentlyChosenMaterials
    /// \return vector of shared pointers to the inventory items as described above
    std::vector<Entity *> filterInventoryForChosenMaterials();
    bool currentRecipeSatisfied();
    void buildItem(Point pos);

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

    std::unique_ptr<CraftingScreenState> mState {std::make_unique<ChoosingRecipeCraftingScreenState>()};
};

struct EquipmentScreen : Screen {
    explicit EquipmentScreen(PlayerEntity &player) : Screen(false), mPlayer(player) {}

    void handleInput(SDL_KeyboardEvent &e) override;

    /// Render equipment screen
    /// \param font Font object to render using
    void render(Font &font) override;

    void reset();
    void enable() override;

    PlayerEntity &getPlayer();

    EquipmentSlot getChosenSlot() const;
    void setChosenSlot(EquipmentSlot chosenSlot);

    void setChoosingEquipmentAction(bool choosingEquipmentAction);
    void setChoosingEquipmentActionIndex(int choosingEquipmentActionIndex);
    void setChoosingNewEquipment(bool choosingNewEquipment);
    void setChoosingNewEquipmentIndex(int choosingNewEquipmentIndex);

private:
    PlayerEntity &mPlayer;
    EquipmentSlot mChosenSlot {EquipmentSlot::HEAD};
    bool mChoosingEquipmentAction {false};
    int mChoosingEquipmentActionIndex {0};
    bool mChoosingNewEquipment {false};
    int mChoosingNewEquipmentIndex {0};

    std::unique_ptr<EquipmentScreenState> mState {std::make_unique<ChoosingSlotEquipmentScreenState>()};
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

struct DebugScreen : Screen {
    DebugScreen() : Screen(false) {
        mState = std::make_unique<ChoosingActionDebugScreenState>();
        mState->onEntry(*this);
    }

    const std::vector<std::string> cDebugOptions = {
        "1) Change time of day"
    };

    void enable() override {
        mChoosingDebugAction = false;
        Screen::enable();
    }

    void handleInput(SDL_KeyboardEvent &e) override;
    void render(Font &font) override;

    void setChoosingDebugAction(bool choosingDebugAction);
    void setChoosingTimeOfDay(bool choosingTimeOfDay);
    void setChosenTime(const Time &chosenTime);

    void setStringPos(int stringPos);

private:
    std::unique_ptr<DebugScreenState> mState;
    bool mChoosingDebugAction {false};
    bool mChoosingTimeOfDay {false};
    int mStringPos {0};

    Time mChosenTime;
};
#endif // DIALOG_H_