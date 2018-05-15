#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"
#include "dialog.h"

struct InventoryScreen;
struct LootingDialog;
class InspectionDialog;
struct CraftingScreen;

struct PlayerEntity : Entity {
    double hunger;
    double hungerRate; // hunger per tick
    double hungerDamageRate; // hp loss per tick while starving
    bool attacking {false}; // whether or not player is attacking something
    bool showingTooMuchWeightMessage {false};
//    bool showingInspectionDialog {false};

    explicit PlayerEntity()
            : Entity("Player", "You, the player", "$[white]$(dwarf)", 10, 10, 0.1, 1, 4, 100),
              hunger(1), hungerRate(0.01), hungerDamageRate(0.15)
    {
        renderingLayer = -1;
    }

    bool attack(const Point& attackPos);
    void tick() override;
    void handleInput(SDL_KeyboardEvent &e, bool &quit, InventoryScreen &inventoryScreen,
                         LootingDialog &lootingDialog, InspectionDialog &inspectionDialog,
                         CraftingScreen &craftingScreen);
    void render(Font &font, Point currentWorldPos) override;
    bool addToInventory(std::shared_ptr<Entity> item) override;
};

struct CatEntity : Entity {
    explicit CatEntity(std::string ID = "")
            : Entity(std::move(ID), "cat", "$[yellow]c", 10, 10, 0.05, 1, 2, 100)
    {
        auto wanderAttach = std::make_shared<WanderAttachBehaviour>(*this, 0.5, 0.7, 0.05);
        auto chaseAndAttack = std::make_shared<ChaseAndAttackBehaviour>(*this, 0.8, 0.6, 8, 8, 0.9);
        chaseAndAttack->enabled = false;
        addBehaviour(wanderAttach);
        addBehaviour(chaseAndAttack);
    }

    void destroy() override;
};

struct EatableEntity : Entity {
    EatableEntity(std::string ID, std::string name, std::string graphic, double hungerRestoration)
            : Entity(std::move(ID), std::move(name), std::move(graphic))
    {
        addBehaviour(std::make_shared<EatableBehaviour>(*this, hungerRestoration));
    }
};

struct FireEntity : Entity {
    explicit FireEntity(std::string ID = "") : Entity(std::move(ID), "Fire", "") {}

    void render(Font &font, Point currentWorldPos) override;
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

struct BandageEntity : Entity {
    const std::string SHORT_DESC = "A rudimentary bandage made of grass";

    explicit BandageEntity(std::string ID = "")
            : Entity(std::move(ID), "Bandage", "$[white]~")
    {
        shortDesc = SHORT_DESC;
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 1));
    }
};

struct BerryEntity;
struct BushEntity : Entity {
    const int RESTOCK_RATE = 100; // ticks

    const std::string SHORT_DESC = "It's a bush!";
    const std::string LONG_DESC = "";

    explicit BushEntity(std::string ID = "") : Entity(std::move(ID), "Bush", "${black}$[purple]$(div)")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        skipLootingDialog = true;
        addBehaviour(std::make_shared<KeepStockedBehaviour<BerryEntity>>(*this, RESTOCK_RATE));
    }

    void render(Font& font, Point currentWorldPos) override;
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

struct GrassEntity : Entity {
    const int RESTOCK_RATE = 200; // ticks

    const std::string SHORT_DESC = "It is dry grass";
    const std::string LONG_DESC = "You can harvest it";

    explicit GrassEntity(std::string ID = "") : Entity(std::move(ID), "Grass", "${black}$[grasshay]$(tau)")
    {
        shortDesc = SHORT_DESC;
        longDesc = LONG_DESC;
        skipLootingDialog = true;
        addBehaviour(std::make_shared<KeepStockedBehaviour<GrassTuftEntity>>(*this, RESTOCK_RATE));
    }

    void render(Font& font, Point currentWorldPos) override;
};

// Food

struct CorpseEntity : EatableEntity {
    CorpseEntity(std::string ID, double hungerRestoration, const std::string& corpseOf, int weight)
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
    }
};

// UI entities

class StatusUIEntity : public Entity {
    const int FORCE_TICK_DISPLAY_LENGTH {100};
    const int SHOW_LOOTED_DISPLAY_LENGTH {200};

    PlayerEntity& player;

    int forceTickDisplayTimer {0};
    int ticksWaitedDuringAnimation {1};
    int attackTargetTimer {0};

    std::string showLootedItemString;
    int showLootedItemDisplayTimer {0};

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
    void showLootedItemNotification(std::string itemString);
};

#endif // ENTITIES_H_
