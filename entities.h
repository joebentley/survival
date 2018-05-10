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

class HealthHungerUIEntity : public Entity {
public:
    PlayerEntity& player;
    bool shown;

    explicit HealthHungerUIEntity(EntityManager& entityManager)
            : Entity("HealthUI", "", entityManager),
              player(dynamic_cast<PlayerEntity&>(*entityManager.getByID("Player"))),
              shown(false) { }

    HealthHungerUIEntity(EntityManager& entityManager, PlayerEntity& player)
            : Entity("HealthUI", "", entityManager),
              player(player),
              shown(false) { }

    void render(Font &font, int currentWorldX, int currentWorldY) override;
};

#endif // ENTITIES_H_