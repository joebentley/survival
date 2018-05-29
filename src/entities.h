#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"
#include "UI.h"
#include "Properties.h"

struct Screen;
enum class ScreenType;

struct PlayerEntity : Entity {
    float hunger;
    float hungerRate; // hunger per tick
    float hungerDamageRate; // hp loss per tick while starving
    bool attacking {false}; // whether or not player is attacking something
    bool showingTooMuchWeightMessage {false};

    explicit PlayerEntity()
            : Entity("Player", "You, the player", "$[white]$(dwarf)", 10.0f, 10.0f, 0.1f, 1, 4, 100),
              hunger(1), hungerRate(0.005f), hungerDamageRate(0.15f)
    {
        mRenderingLayer = -1;
    }

    bool attack(const Point& attackPos);
    void tick() override;
    void handleInput(SDL_KeyboardEvent &e, bool &quit, std::unordered_map<ScreenType, std::shared_ptr<Screen>> &screens);
    void render(Font &font, Point currentWorldPos) override;
    bool addToInventory(const std::string &ID) override;

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
    const std::string SHORT_DESC = "A yellow fruit found in the jungle. The shape looks familiar...";
    const std::string LONG_DESC = "This fruit was discovered in [redacted]. They were brought west by Arab conquerors in 327 B.C.";

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
