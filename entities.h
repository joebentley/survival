#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"

class PlayerEntity : public Entity {
public:
    int hunger;

    explicit PlayerEntity(EntityManager& entityManager)
            : Entity("Player", "$[white]$(dwarf)", entityManager, 10, 10), hunger(10)
    {
        std::shared_ptr<Behaviour> behaviour = std::make_shared<PlayerInputBehaviour>(*this);
        addBehaviour(behaviour);
    }

    void attack(const Point& attackPos);
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
    void emit(uint32_t signal) override;
};

#endif // ENTITIES_H_