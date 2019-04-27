#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "Entity.h"
#include "Behaviours.h"
#include "UI.h"
#include "Properties.h"

#include <unordered_set>

struct Screen;
enum class ScreenType;

/// A special entity with ID "Player"
struct PlayerEntity : Entity {
    /// 1 is full, <0.3 is starving
    float hunger;
    /// how much hunger should decrease per tick
    float hungerRate;
    /// hp loss per tick while starving
    float hungerDamageRate;
    /// whether or not player is attacking something
    bool attacking {false};
    /// whether or not showing "too much weight message" on screen
    bool showingTooMuchWeightMessage {false};

    explicit PlayerEntity()
            : Entity("Player", "You, the player", "$[white]$(dwarf)", 10.0f, 10.0f, 0.1f, 1, 4, 100),
              hunger(1), hungerRate(0.005f), hungerDamageRate(0.15f)
    {
        mRenderingLayer = -1;
    }

    /// Try to attack the entity at attackPos
    bool attack(const Point& attackPos);
    /// Tick entity, taking hunger into account
    void tick() override;
    /// Handle most of the ingame interaction
    void handleInput(SDL_KeyboardEvent &e, bool &quit, std::unordered_map<ScreenType, std::unique_ptr<Screen>> &screens);
    /// Same as usual render except can show a "too much weight" message box, and offload additional rendering to "mEntityInteractingWith"
    void render(Font &font, Point currentWorldPos) override;
    /// Overrides usual entity addToInventory but displays a nice notification
    bool addToInventory(const std::string &ID) override;

    /// Add hunger, not exceeding 1.0f
    void addHunger(float hunger);

private:
    bool interactingWithEntity {false};
    std::string mEntityInteractingWith;
};

// AI entities

struct CatEntity : Entity {
    explicit CatEntity(std::string ID = "")
            : Entity(std::move(ID), "Cat", "$[yellow]c", 10.0f, 10.0f, 0.05f, 1, 2, 100)
    {
        auto wanderAttach = std::make_unique<WanderAttachBehaviour>(*this, 0.5f, 0.7f, 0.05f);
        auto chaseAndAttack = std::make_unique<ChaseAndAttackBehaviour>(*this, 0.8f, 0.1f, 8.0f, 8.0f, 0.9f);
        chaseAndAttack->disable();
        addBehaviour(std::move(wanderAttach));
        addBehaviour(std::move(chaseAndAttack));
    }

    void destroy() override;
};

struct WolfEntity : Entity {
    explicit WolfEntity(std::string ID = "")
            : Entity(std::move(ID), "Wolf", "${black}$[red]W", 20.0f, 20.0f, 0.05f, 1, 4, 100)
    {
        addBehaviour(std::make_unique<WanderBehaviour>(*this));
        auto chaseAndAttack = std::make_unique<ChaseAndAttackBehaviour>(*this, 0.8f, 0.05f, 8, 8, 0.9f);
        chaseAndAttack->disable();
        addBehaviour(std::move(chaseAndAttack));
        addBehaviour(std::make_unique<HostilityBehaviour>(*this, 12, 0.95f));

        mShortDesc = "A terrifying looking beast!";
    }

    void destroy() override;
};

struct GlowbugEntity : Entity {
    explicit GlowbugEntity(std::string ID = "")
            : Entity(std::move(ID), "Glowbug", "$[green]`", 10.0f, 10.0f, 0.05f)
    {
        addBehaviour(std::make_unique<WanderBehaviour>(*this));
        addProperty(std::make_unique<LightEmittingProperty>(this, 3, FontColor::getColor("green")));
    }

    void render(Font &font, Point currentWorldPos) override;
};

// Base item entities

struct EatableEntity : Entity {
    EatableEntity(std::string ID, std::string name, std::string graphic, float hungerRestoration)
            : Entity(std::move(ID), std::move(name), std::move(graphic))
    {
        addProperty(std::make_unique<EatableProperty>(hungerRestoration));
    }
};

// Regrowing entities (bushes, trees, etc.)

struct BerryEntity;
struct BushEntity : Entity {
    const int RESTOCK_RATE = 200; // ticks

    const std::string SHORT_DESC = "It's a bush!";
    const std::string LONG_DESC = "";

    explicit BushEntity(std::string ID = "");

    void render(Font& font, Point currentWorldPos) override;
};

struct GrassTuftEntity;
struct GrassEntity : Entity {
    const int RESTOCK_RATE = 100; // ticks

    const std::string SHORT_DESC = "It is dry grass";
    const std::string LONG_DESC = "You can harvest it";

    explicit GrassEntity(std::string ID = "");

    void render(Font& font, Point currentWorldPos) override;
};

// Food items

struct CorpseEntity : EatableEntity {
    CorpseEntity(std::string ID, float hungerRestoration, const std::string& corpseOf, int weight)
            : EatableEntity(std::move(ID), "Corpse of " + corpseOf, "${black}$[red]x", hungerRestoration)
    {
        addProperty(std::make_unique<PickuppableProperty>(weight));
    }
};

struct AppleEntity : EatableEntity {
    const std::string SHORT_DESC = "A small, fist-sized fruit that is hopefully crispy and juicy";
    const std::string LONG_DESC = "This is a longer description of the apple";

    explicit AppleEntity(std::string ID = "")
            : EatableEntity(std::move(ID), "Apple", "$[green]a", 0.5)
    {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};

struct BananaEntity : EatableEntity {
    const std::string SHORT_DESC = "A yellow fruit found in the jungle.";
    const std::string LONG_DESC = "This fruit was discovered in . They were brought west by Arab conquerors in 327 B.C.";

    explicit BananaEntity(std::string ID = "")
            : EatableEntity(std::move(ID), "Banana", "$[yellow]b", 0.5)
    {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};

struct BerryEntity : EatableEntity {
    const std::string SHORT_DESC = "A purple berry";
    const std::string LONG_DESC = "";

    explicit BerryEntity(std::string ID = "")
    : EatableEntity(std::move(ID), "Berry", "$[purple]$(male)", 0.5)
    {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};

// Healing items

struct BandageEntity : Entity {
    const std::string SHORT_DESC = "A rudimentary bandage made of grass";

    explicit BandageEntity(std::string ID = "")
            : Entity(std::move(ID), "Bandage", "$[white]~")
    {
        mShortDesc = SHORT_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
        addBehaviour(std::make_unique<HealingItemBehaviour>(*this, 5));
    }
};

// Materials

struct TwigEntity : Entity {
    const std::string SHORT_DESC = "A thin, brittle twig";
    const std::string LONG_DESC = "It looks very useful! Who knows where it came from...";

    explicit TwigEntity(std::string ID = "")
    : Entity(std::move(ID), "Twig", "${black}$[brown]/")
    {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<CraftingMaterialProperty>("wood", 1));
        addProperty(std::make_unique<EquippableProperty>(
                std::vector<EquipmentSlot> {EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND}));
//        addBehaviour(std::make_unique<MeleeWeaponBehaviour>(*this, 1));
        addProperty(std::make_unique<MeleeWeaponDamageProperty>(1));
    }
};

struct GrassTuftEntity : Entity {
    const std::string SHORT_DESC = "A tuft of dry grass";
    const std::string LONG_DESC = "A tuft of dry grass, very useful";

    explicit GrassTuftEntity(std::string ID = "") : Entity(std::move(ID), "Tuft of grass", "$[grasshay]$(approx)")
    {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<CraftingMaterialProperty>("grass", 1));
    }
};

// Misc entities

struct FireEntity : Entity {
    struct RekindleBehaviour : InteractableBehaviour {
        explicit RekindleBehaviour(Entity& parent) : InteractableBehaviour(parent) {}

        bool handleInput(SDL_KeyboardEvent &e) override;
        void render(Font &font) override;

    private:
        bool choosingItemToUse {false};
        int choosingItemIndex {0};
    };

    explicit FireEntity(std::string ID = "") : Entity(std::move(ID), "Fire", "") {
        mIsSolid = true;
        addProperty(std::make_unique<LightEmittingProperty>(this, 6));
        addBehaviour(std::make_unique<RekindleBehaviour>(*this));
    }

    void render(Font &font, Point currentWorldPos) override;
    void tick() override;

    float fireLevel {1};
};

struct TorchEntity : Entity {
    explicit TorchEntity(std::string ID = "") : Entity(std::move(ID), "Torch", "$[red]$(up)") {
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<LightEmittingProperty>(this, 4));
        addProperty(std::make_unique<EquippableProperty>(
                std::vector<EquipmentSlot> {EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND }));

        mShortDesc = "Can be equipped to produce light and some heat.";
    }
};

struct ChestEntity : Entity {
    const std::string SHORT_DESC = "A heavy wooden chest";
    const std::string LONG_DESC = "This chest is super heavy";

    explicit ChestEntity(std::string ID = "")
            : Entity(std::move(ID), "Chest", "${black}$[brown]$(accentAE)")
    {
        mShortDesc = SHORT_DESC;
        mLongDesc = LONG_DESC;

        // TODO: allow player to place items in chest
    }
};

struct BagEntity : Entity {
    explicit BagEntity(std::string ID = "")
            : Entity(std::move(ID), "Grass Bag", "$[green]$(Phi)")
    {
        mShortDesc = "This crude grass bag allows you to carry a few more items";
        addProperty(std::make_unique<PickuppableProperty>(1));
        addProperty(std::make_unique<EquippableProperty>(EquipmentSlot::BACK));
        addProperty(std::make_unique<AdditionalCarryWeightProperty>(20));
    }
};

struct WaterEntity : Entity {
    explicit WaterEntity(std::string ID = "")
            : Entity(std::move(ID), "Water", "")
    {
        mRenderingLayer = 10;

        if (randDouble() > 0.5)
            mGraphic = "${black}$[cyan]$(approx)";
        else
            mGraphic = "${black}$[cyan]~";
    }
};

// Water containers

struct WaterskinEntity : Entity {
    explicit WaterskinEntity() : Entity("", "Waterskin", "$[brown]$(male)")
    {
        addProperty(std::make_unique<WaterContainerProperty>());
        addProperty(std::make_unique<PickuppableProperty>(1));
    }
};

// Building entities

/// Generates a set of walls for a building from a layout string.
/// The walls and corners are automatically detected to use the corresponding double-thickness pipe characters.
struct BuildingWallEntity : Entity {
    /// Initialize a new building position entity from a layout string
    /// e.g.
    /// std::vector<std::string> layout = {
    ///     "xxxxx",
    ///     "x   x",
    ///     "x   x",
    ///     "x   x",
    ///     "xx xx"
    /// }
    /// would be a small hut with a door.
    /// \param pos world position of top left corner of building
    /// \param layout vector of strings, each line being a row of the building. Each cross 'x' becomes a wall of the building
    explicit BuildingWallEntity(const Point &pos, const std::vector<std::string> &layout);

    /// Overrides
    /// \param font
    /// \param currentWorldPos
    void render(Font& font, Point currentWorldPos) override;

    /// Overrides collision detection to take the numerous walls into account
    /// \param pos position to check if colliding
    /// \return whether or not collision occurred
    bool collide(const Point &pos) override;

    /// The different allowed wall types
    enum class WallType : int {
        UL_CORNER, // corner with adjacent walls above and to the left
        UR_CORNER, // corner w/ walls above and to the right
        DL_CORNER, // corner w/ walls below and to the left
        DR_CORNER, // corner w/ walls below and to the right
        VERT,      // vertical wall with walls above and below
        HORIZ,     // horizontal wall with walls to left and right
        CROSS,     // 4 way corner with walls in every direction
        T_UP,      // T junction with walls above, to left, and to right
        T_LEFT,    // T junction with walls above, to left, and down
        T_DOWN,    // T junction with walls to left, down, and to right
        T_RIGHT    // T junction with walls down, to right, and up
    };

private:
    std::unordered_map<Point, WallType> mWalls;

    /// Take a set of points and populate mWalls with the correct WallTypes
    /// \param points the set of points that represent the building
    void generateWallsFromPoints(const std::unordered_set<Point> &points);
};

// UI entities

class StatusUIEntity : public Entity {
    const int FORCE_TICK_DISPLAY_LENGTH {100};

    PlayerEntity& player;

    int forceTickDisplayTimer {0};
    int ticksWaitedDuringAnimation {1};
    int attackTargetTimer {0};

    std::string mAttackTargetID;
    const int X_OFFSET = 10;

public:
    StatusUIEntity()
            : StatusUIEntity(dynamic_cast<PlayerEntity&>(*EntityManager::getInstance().getEntityByID("Player"))) { }

    explicit StatusUIEntity(PlayerEntity& player)
            : Entity("StatusUI", "", ""), player(player)
    {
        mRenderingLayer = 1; // Keep on background
    }

    void render(Font &font, Point currentWorldPos) override;
    void emit(Uint32 signal) override;
    void tick() override;
    void setAttackTarget(const std::string &attackTargetID) {
        mAttackTargetID = attackTargetID;
        attackTargetTimer = 10;
    }
    void clearAttackTarget() {
        mAttackTargetID.clear();
    }
//    void showLootedItemNotification(std::string itemString);
};

#endif // ENTITIES_H_
