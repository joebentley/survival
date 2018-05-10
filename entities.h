#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"

class PlayerEntity : public Entity {
public:
    int hp;
    int maxhp;
    int hunger;

    explicit PlayerEntity(EntityManager& entityManager)
            : Entity("Player", "$[white]$(dwarf)", entityManager), hp(10), maxhp(10), hunger(10)
    {
        std::unique_ptr<Behaviour> behaviour = std::make_unique<PlayerInputBehaviour>(*this);
        addBehaviour(behaviour);
    }
};

class StatusUIEntity : public Entity {
public:
    PlayerEntity& player;
    bool shown;
    int forceTickDisplayTimer;
    int ticksWaitedDuringAnimation;

    const int X_OFFSET = 10;

    explicit StatusUIEntity(EntityManager& entityManager)
            : StatusUIEntity(entityManager, dynamic_cast<PlayerEntity&>(*entityManager.getByID("Player"))) { }

    StatusUIEntity(EntityManager& entityManager, PlayerEntity& player)
            : Entity("HealthUI", "", entityManager),
              player(player), shown(false), forceTickDisplayTimer(0), ticksWaitedDuringAnimation(1) { }

    void render(Font &font, int currentWorldX, int currentWorldY) override;
    void emit(const std::string &event) override;
};

#endif // ENTITIES_H_