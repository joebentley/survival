#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"
#include "UI.h"

struct InventoryScreen;
struct LootingDialog;
class InspectionDialog;
struct CraftingScreen;
struct EquipmentScreen;
struct NotificationMessageScreen;
struct HelpScreen;

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
        renderingLayer = -1;
    }

    bool attack(const Point& attackPos);
    void tick() override;
    void handleInput(SDL_KeyboardEvent &e, bool &quit, InventoryScreen &inventoryScreen,
                         LootingDialog &lootingDialog, InspectionDialog &inspectionDialog,
                         CraftingScreen &craftingScreen, EquipmentScreen &equipmentScreen,
                         NotificationMessageScreen &notificationMessageScreen, HelpScreen &helpScreen);
    void render(Font &font, Point currentWorldPos) override;
    bool addToInventory(const std::shared_ptr<Entity> &item) override;

    void addHunger(float hunger);

private:
    bool interactingWithEntity {false};
    std::shared_ptr<Entity> entityInteractingWith {nullptr};
};

// AI entities

struct CatEntity : Entity {
    explicit CatEntity(std::string ID = "")
            : Entity(std::move(ID), "Cat", "$[yellow]c", 10.0f, 10.0f, 0.05f, 1, 2, 100)
    {
        auto wanderAttach = std::make_shared<WanderAttachBehaviour>(*this, 0.5f, 0.7f, 0.05f);
        auto chaseAndAttack = std::make_shared<ChaseAndAttackBehaviour>(*this, 0.8f, 0.1f, 8.0f, 8.0f, 0.9f);
        chaseAndAttack->disable();
        addBehaviour(wanderAttach);
        addBehaviour(chaseAndAttack);
    }

    void destroy() override;
};

struct WolfEntity : Entity {
    explicit WolfEntity(std::string ID = "")
            : Entity(std::move(ID), "Wolf", "${black}$[red]W", 20.0f, 20.0f, 0.05f, 1, 4, 100)
    {
        addBehaviour(std::make_shared<WanderBehaviour>(*this));
        auto chaseAndAttack = std::make_shared<ChaseAndAttackBehaviour>(*this, 0.8f, 0.05f, 8, 8, 0.9f);
        chaseAndAttack->disable();
        addBehaviour(chaseAndAttack);
        addBehaviour(std::make_shared<HostilityBehaviour>(*this, 12, 0.95f));

        shortDesc = "A terrifying looking beast!";
    }

    void destroy() override;
};

struct GlowbugEntity : Entity {
    explicit GlowbugEntity(std::string ID = "")
            : Entity(std::move(ID), "Glowbug", "$[green]`", 10.0f, 10.0f, 0.05f)
    {
        addBehaviour(std::make_shared<WanderBehaviour>(*this));
        addBehaviour(std::make_shared<LightEmittingBehaviour>(*this, 3, getColor("green")));
    }

    void render(Font &font, Point currentWorldPos) override;
};

// Base item entities

struct EatableEntity : Entity {
    EatableEntity(std::string ID, std::string name, std::string graphic, float hungerRestoration)
            : Entity(std::move(ID), std::move(name), std::move(graphic))
    {
        addBehaviour(std::make_shared<EatableBehaviour>(*this, hungerRestoration));
    }
};

// Regrowing entities (bushes, trees, etc.)

struct BerryEntity;
struct BushEntity : Entity {
    const int RESTOCK_RATE = 200; // ticks

    const std::string SHORT_DESC = "It's a bush!";
    const std::string LONG_DESC = "";

    explicit BushEntity(std::string ID = "") : Entity(std::move(ID), "Bush", "${black}$[purple]$(div)")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        skipLootingDialog = true;
        addBehaviour(std::make_shared<KeepStockedBehaviour<BerryEntity>>(*this, RESTOCK_RATE));
        addToInventory(std::dynamic_pointer_cast<Entity>(std::make_shared<BerryEntity>()));
    }

    void render(Font& font, Point currentWorldPos) override;
};

struct GrassTuftEntity;
struct GrassEntity : Entity {
    const int RESTOCK_RATE = 100; // ticks

    const std::string SHORT_DESC = "It is dry grass";
    const std::string LONG_DESC = "You can harvest it";

    explicit GrassEntity(std::string ID = "") : Entity(std::move(ID), "Grass", "${black}$[grasshay]$(tau)")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        skipLootingDialog = true;
        addBehaviour(std::make_shared<KeepStockedBehaviour<GrassTuftEntity>>(*this, RESTOCK_RATE));
        addToInventory(std::dynamic_pointer_cast<Entity>(std::make_shared<GrassTuftEntity>()));
    }

    void render(Font& font, Point currentWorldPos) override;
};

// Food items

struct CorpseEntity : EatableEntity {
    CorpseEntity(std::string ID, float hungerRestoration, const std::string& corpseOf, int weight)
            : EatableEntity(std::move(ID), "Corpse of " + corpseOf, "${black}$[red]x", hungerRestoration)
    {
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, weight));
    }
};

struct AppleEntity : EatableEntity {
    const std::string SHORT_DESC = "A small, fist-sized fruit that is hopefully crispy and juicy";
    const std::string LONG_DESC = "This is a longer description of the apple";

    explicit AppleEntity(std::string ID = "")
            : EatableEntity(std::move(ID), "Apple", "$[green]a", 0.5)
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
    }
};

struct BananaEntity : EatableEntity {
    const std::string SHORT_DESC = "A yellow fruit found in the jungle. The shape looks familiar...";
    const std::string LONG_DESC = "This fruit was discovered in [redacted]. They were brought west by Arab conquerors in 327 B.C.";

    explicit BananaEntity(std::string ID = "")
            : EatableEntity(std::move(ID), "Banana", "$[yellow]b", 0.5)
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
    }
};

struct BerryEntity : EatableEntity {
    const std::string SHORT_DESC = "A purple berry";
    const std::string LONG_DESC = "";

    explicit BerryEntity(std::string ID = "")
    : EatableEntity(std::move(ID), "Berry", "$[purple]$(male)", 0.5)
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
    }
};

// Healing items

struct BandageEntity : Entity {
    const std::string SHORT_DESC = "A rudimentary bandage made of grass";

    explicit BandageEntity(std::string ID = "")
            : Entity(std::move(ID), "Bandage", "$[white]~")
    {
        shortDesc = SHORT_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
        addBehaviour(std::make_shared<HealingItemBehaviour>(*this, 5));
    }
};

// Materials

struct TwigEntity : Entity {
    const std::string SHORT_DESC = "A thin, brittle twig";
    const std::string LONG_DESC = "It looks very useful! Who knows where it came from...";

    explicit TwigEntity(std::string ID = "")
    : Entity(std::move(ID), "Twig", "${black}$[brown]/")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
        addBehaviour(std::make_shared<CraftingMaterialBehaviour>(*this, "wood", 1));
        addBehaviour(std::make_shared<EquippableBehaviour>(*this,
                std::vector<EquipmentSlot> {EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND}));
        addBehaviour(std::make_shared<MeleeWeaponBehaviour>(*this, 1));
    }
};

struct GrassTuftEntity : Entity {
    const std::string SHORT_DESC = "A tuft of dry grass";
    const std::string LONG_DESC = "A tuft of dry grass, very useful";

    explicit GrassTuftEntity(std::string ID = "") : Entity(std::move(ID), "Tuft of grass", "$[grasshay]$(approx)")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
        addBehaviour(std::make_shared<CraftingMaterialBehaviour>(*this, "grass", 1));
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
        isSolid = true;
        addBehaviour(std::make_shared<LightEmittingBehaviour>(*this, 6));
        addBehaviour(std::make_shared<RekindleBehaviour>(*this));
    }

    void render(Font &font, Point currentWorldPos) override;
    void tick() override;

    float fireLevel {1};
};

struct TorchEntity : Entity {
    explicit TorchEntity(std::string ID = "") : Entity(std::move(ID), "Torch", "$[red]$(up)") {
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
        addBehaviour(std::make_shared<LightEmittingBehaviour>(*this, 4));
        addBehaviour(std::make_shared<EquippableBehaviour>(*this,
                std::vector<EquipmentSlot> {EquipmentSlot::LEFT_HAND, EquipmentSlot::RIGHT_HAND }));

        shortDesc = "Can be equipped to produce light and some heat.";
    }
};

struct ChestEntity : Entity {
    const std::string SHORT_DESC = "A heavy wooden chest";
    const std::string LONG_DESC = "This chest is super heavy";

    explicit ChestEntity(std::string ID = "")
            : Entity(std::move(ID), "Chest", "${black}$[brown]$(accentAE)")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;

        // TODO: allow player to place items in chest
    }
};

struct BagEntity : Entity {
    explicit BagEntity(std::string ID = "")
            : Entity(std::move(ID), "Grass Bag", "$[green]$(Phi)")
    {
        shortDesc = "This crude grass bag allows you to carry a few more items";
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
        addBehaviour(std::make_shared<EquippableBehaviour>(*this, EquipmentSlot::BACK));
        addBehaviour(std::make_shared<AdditionalCarryWeightBehaviour>(*this, 20));
    }
};

struct WaterEntity : Entity {
    explicit WaterEntity(std::string ID = "")
            : Entity(std::move(ID), "Water", "")
    {
        renderingLayer = 10;

        if (randDouble() > 0.5)
            graphic = "${black}$[cyan]$(approx)";
        else
            graphic = "${black}$[cyan]~";
    }
};

// Water containers

struct WaterskinEntity : Entity {
    explicit WaterskinEntity() : Entity("", "Waterskin", "$[brown]$(male)")
    {
        addBehaviour(std::make_shared<WaterContainerBehaviour>(*this));
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
    }
};

// UI entities

class StatusUIEntity : public Entity {
    const int FORCE_TICK_DISPLAY_LENGTH {100};

    PlayerEntity& player;

    int forceTickDisplayTimer {0};
    int ticksWaitedDuringAnimation {1};
    int attackTargetTimer {0};

    std::shared_ptr<Entity> attackTarget { nullptr };
    const int X_OFFSET = 10;

public:
    StatusUIEntity()
            : StatusUIEntity(dynamic_cast<PlayerEntity&>(*EntityManager::getInstance().getEntityByID("Player"))) { }

    explicit StatusUIEntity(PlayerEntity& player)
            : Entity("StatusUI", "", ""), player(player)
    {
        renderingLayer = 1; // Keep on background
    }

    void render(Font &font, Point currentWorldPos) override;
    void emit(Uint32 signal) override;
    void tick() override;
    void setAttackTarget(std::shared_ptr<Entity> attackTarget) {
        this->attackTarget = std::move(attackTarget);
        attackTargetTimer = 10;
    }
    void clearAttackTarget() {
        this->attackTarget = nullptr;
    }
//    void showLootedItemNotification(std::string itemString);
};

#endif // ENTITIES_H_
