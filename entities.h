#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"

class PlayerEntity : public Entity {
public:
    double hunger;
    double hungerRate; // hunger per tick
    double hungerDamageRate; // hp loss per tick while starving
    bool attacking; // whether or not player is attacking something

    explicit PlayerEntity(EntityManager& entityManager)
            : Entity("Player", "$[white]$(dwarf)", entityManager, 10, 10, 0.1, 1, 4), hunger(1), hungerRate(0.01), hungerDamageRate(0.15)
    {
        renderingLayer = -1;
    }

    bool attack(const Point& attackPos);
    void tick() override;
    void emit(Uint32 signal) override;
};

class CatEntity : public Entity {
public:
    CatEntity(std::string ID, EntityManager& entityManager)
            : Entity(std::move(ID), "$[yellow]c", entityManager, 10, 10, 0.05)
    {
        std::shared_ptr<Behaviour> wanderAttach = std::make_shared<WanderAttachBehaviour>(*this, 0.5, 0.7, 0.05);
        std::shared_ptr<Behaviour> chaseAndAttack = std::make_shared<ChaseAndAttackBehaviour>(*this, 0.8, 0.6, 8, 8, 0.9);
        chaseAndAttack->enabled = false;
        addBehaviour(wanderAttach);
        addBehaviour(chaseAndAttack);
    }
};

class StatusUIEntity : public Entity {
public:
    PlayerEntity& player;
    bool shown;
    int forceTickDisplayTimer;
    int ticksWaitedDuringAnimation;

    std::shared_ptr<Entity> attackTarget { nullptr };

    const int X_OFFSET = 10;

    explicit StatusUIEntity(EntityManager& entityManager)
            : StatusUIEntity(entityManager, dynamic_cast<PlayerEntity&>(*entityManager.getByID("Player"))) { }

    StatusUIEntity(EntityManager& entityManager, PlayerEntity& player)
            : Entity("StatusUI", "", entityManager),
              player(player), shown(false), forceTickDisplayTimer(0), ticksWaitedDuringAnimation(1) { }

    void render(Font &font, int currentWorldX, int currentWorldY) override;
    void emit(Uint32 signal) override;
};

#endif // ENTITIES_H_