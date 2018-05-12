#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"
#include "dialog.h"

class InventoryScreen;
class LootingDialog;

class PlayerEntity : public Entity {
public:
    double hunger;
    double hungerRate; // hunger per tick
    double hungerDamageRate; // hp loss per tick while starving
    bool attacking {false}; // whether or not player is attacking something

    explicit PlayerEntity(EntityManager& entityManager)
            : Entity(entityManager, "Player", "player", "You, the player", "$[white]$(dwarf)", 10, 10, 0.1, 1, 4), hunger(1), hungerRate(0.01), hungerDamageRate(0.15)
    {
        renderingLayer = -1;
    }

    bool attack(const Point& attackPos);
    void tick() override;
    void handleInput(SDL_KeyboardEvent &e, bool &quit, InventoryScreen &inventoryScreen, LootingDialog &lootingDialog);
};

class CatEntity : public Entity {
public:
    CatEntity(EntityManager& entityManager, std::string ID)
            : Entity(entityManager, std::move(ID), "living", "cat", "$[yellow]c", 10, 10, 0.05, 1, 2)
    {
        auto wanderAttach = std::make_shared<WanderAttachBehaviour>(*this, 0.5, 0.7, 0.05);
        auto chaseAndAttack = std::make_shared<ChaseAndAttackBehaviour>(*this, 0.8, 0.6, 8, 8, 0.9);
        chaseAndAttack->enabled = false;
        addBehaviour(wanderAttach);
        addBehaviour(chaseAndAttack);
    }
};

class EatableEntity : public Entity {
public:
    EatableEntity(EntityManager& entityManager, std::string ID, std::string type, std::string name, std::string graphic, double hungerRestoration)
            : Entity(entityManager, std::move(ID), std::move(type), std::move(name), std::move(graphic))
    {
        addBehaviour(std::make_shared<EatableBehaviour>(*this, hungerRestoration));
    }
};

class CorpseEntity : public EatableEntity {
public:
    CorpseEntity(EntityManager& entityManager, std::string ID, double hungerRestoration, const std::string& corpseOf)
            : EatableEntity(entityManager, std::move(ID), "corpse", "Corpse of " + corpseOf, "${black}$[red]x", hungerRestoration)
    {
        addBehaviour(std::make_shared<PickuppableBehaviour>(*this, 100));
    }
};

class StatusUIEntity : public Entity {
public:
    PlayerEntity& player;
    bool shown;
    int forceTickDisplayTimer;
    int ticksWaitedDuringAnimation;
    int attackTargetTimer {0};

    std::shared_ptr<Entity> attackTarget { nullptr };

    const int X_OFFSET = 10;

    explicit StatusUIEntity(EntityManager& entityManager)
            : StatusUIEntity(entityManager, dynamic_cast<PlayerEntity&>(*entityManager.getEntityByID("Player"))) { }

    StatusUIEntity(EntityManager& entityManager, PlayerEntity& player)
            : Entity(entityManager, "StatusUI", "UI", "", ""),
              player(player), shown(false), forceTickDisplayTimer(0), ticksWaitedDuringAnimation(1) { }

    void render(Font &font, int currentWorldX, int currentWorldY) override;
    void emit(Uint32 signal) override;
    void tick() override;
    void setAttackTarget(std::shared_ptr<Entity> attackTarget) {
        this->attackTarget = std::move(attackTarget);
        attackTargetTimer = 10;
    }
};

#endif // ENTITIES_H_