#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"

class PlayerEntity : public Entity {
public:
    int hp;
    int maxhp;
    int hunger;

    PlayerEntity(EntityManager& entityManager) : Entity("Player", "$[white]$(dwarf)", entityManager), hp(10), maxhp(10), hunger(10) {
        std::unique_ptr<Behaviour> behaviour = std::make_unique<PlayerInputBehaviour>(*this);
        addBehaviour(behaviour);
    }
};

class HealthUIEntity : public Entity {
public:
    PlayerEntity& player;
    bool shown;

    HealthUIEntity(EntityManager& entityManager)
            : Entity("HealthUI", "", entityManager),
              player(dynamic_cast<PlayerEntity&>(*entityManager.getByID("Player"))),
              shown(false) { }

    HealthUIEntity(EntityManager& entityManager, PlayerEntity& player)
            : Entity("HealthUI", "", entityManager),
              player(player),
              shown(false) { }

    void render(Font &font, int currentWorldX, int currentWorldY);
};

#endif // ENTITIES_H_