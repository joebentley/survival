#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "entity.h"
#include "behaviours.h"

class PlayerEntity : public Entity {
public:
    int hp;

    PlayerEntity(EntityManager& entityManager) : Entity("Player", "$[white]$(dwarf)", entityManager) {
        PlayerInputBehaviour *behaviour = new PlayerInputBehaviour(*this);
        addBehaviour(behaviour);
    }

    ~PlayerEntity () {
        free(behaviours[0]);
    }
};

#endif // ENTITIES_H_