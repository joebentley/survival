#pragma once
#include "../../utils.h"
#include "AttachmentBehaviour.h"
#include "WanderBehaviour.h"

/// Combination of a WanderBehaviour and AttachBehaviour, with a random probability to go from wander to attach
struct WanderAttachBehaviour : Behaviour {
    WanderBehaviour wander;
    AttachmentBehaviour attach;
    /// Only wander, don't attach
    bool onlyWander{false};

    /// Initialize the behaviour, giving parameters for the AttachBehaviour
    /// \param parent parent of this entity
    /// \param attachment probability of attaching to player
    /// \param clinginess probability of moving to player when attached
    /// \param unattachment probability of unattaching from player
    /// \param range how close we need to be to attach
    WanderAttachBehaviour(Entity &parent, float attachment, float clinginess, float unattachment, float range)
        : Behaviour("WanderAttachBehaviour", parent), wander(parent),
          attach(parent, attachment, clinginess, unattachment, range) {}

    /// Initialize with a range of 10
    WanderAttachBehaviour(Entity &parent, float attachment, float clinginess, float unattachment)
        : WanderAttachBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick() override {
        if (onlyWander) {
            wander.tick();
            return;
        }

        double r = randDouble();

        // If attached, wander OR follow not both at once
        if (attach.attached) {
            if (r < attach.clinginess) {
                attach.tick();
            } else {
                wander.tick();
            }
        } else {
            // decide whether to reattach...
            attach.tick();
            // ...then wander
            wander.tick();
        }
    }
};