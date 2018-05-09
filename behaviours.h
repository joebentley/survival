#ifndef BEHAVIOURS_H_
#define BEHAVIOURS_H_

#include "entity.h"

class PlayerInputBehaviour : public Behaviour {
public:
    PlayerInputBehaviour(Entity& parent) : Behaviour("PlayerInputBehaviour", parent) {}
    void handle(const std::string& event);
};

#endif // BEHAVIOURS_H_