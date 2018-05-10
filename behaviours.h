#ifndef BEHAVIOURS_H_
#define BEHAVIOURS_H_

#include "entity.h"

class WanderBehaviour : public Behaviour {
public:
    WanderBehaviour(Entity& parent) : Behaviour("WanderBehaviour", parent) {}
    void tick();
};

class AttachmentBehaviour : public Behaviour {
public:
    float attachment; // probability of attaching to player
    float clinginess; // probability of moving to player
    float unattachment; // probability of unattaching from player
    float range; // range needed to be within player to attach
    bool attached; // whether or not is attached to player

    AttachmentBehaviour(Entity& parent, float attachment, float clinginess, float unattachment, float range)
        : Behaviour("AttachmentBehaviour", parent),
          attachment(attachment), clinginess(clinginess), unattachment(unattachment), range(range) {}

    AttachmentBehaviour(Entity& parent, float attachment, float clinginess, float unattachment)
        : AttachmentBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick();
};

class PlayerInputBehaviour : public Behaviour {
public:
    PlayerInputBehaviour(Entity& parent) : Behaviour("PlayerInputBehaviour", parent) {}
    void handle(const std::string& event);
};

#endif // BEHAVIOURS_H_