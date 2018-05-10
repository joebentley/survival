#ifndef BEHAVIOURS_H_
#define BEHAVIOURS_H_

#include "entity.h"

class WanderBehaviour : public Behaviour {
public:
    explicit WanderBehaviour(Entity& parent) : Behaviour("WanderBehaviour", parent) {}
    void tick() override;
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
          attachment(attachment), clinginess(clinginess), unattachment(unattachment), range(range), attached(false) {}

    AttachmentBehaviour(Entity& parent, float attachment, float clinginess, float unattachment)
        : AttachmentBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick() override;
};

class WanderAttachBehaviour : public Behaviour {
public:
    WanderBehaviour wander;
    AttachmentBehaviour attach;

    WanderAttachBehaviour(Entity& parent, float attachment, float clinginess, float unattachment, float range)
            : Behaviour("WanderAttachBehaviour", parent), wander(parent), attach(parent, attachment, clinginess, unattachment, range) {}

    WanderAttachBehaviour(Entity& parent, float attachment, float clinginess, float unattachment)
            : WanderAttachBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick() override {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        // If attached, wander OR follow not both at once
        if (attach.attached) {
            if (r < attach.clinginess) {
                attach.tick();
            } else {
                wander.tick();
            }
        } else {
            attach.tick();
            wander.tick();
        }
    }
};

class PlayerInputBehaviour : public Behaviour {
public:
    explicit PlayerInputBehaviour(Entity& parent) : Behaviour("PlayerInputBehaviour", parent) {}
    void handle(uint32_t signal) override;
};

#endif // BEHAVIOURS_H_