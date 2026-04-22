#pragma once

#include "../../Entity/Entity.h"
#include "../Behaviour.h"

/// This behaviour causes the entity to randomly attach to and follow the entity with ID "Player"
struct AttachmentBehaviour : Behaviour {
    float attachment;
    float clinginess;
    float unattachment;
    float range;
    /// whether or not is currently attached to player
    bool attached;

    /// Initialize the attachment behaviour
    /// \param parent parent of this entity
    /// \param attachment probability of attaching to player
    /// \param clinginess probability of moving to player when attached
    /// \param unattachment probability of unattaching from player
    /// \param range how close we need to be to attach
    AttachmentBehaviour(Entity &parent, float attachment, float clinginess, float unattachment, float range)
        : Behaviour("AttachmentBehaviour", parent), attachment(attachment), clinginess(clinginess),
          unattachment(unattachment), range(range), attached(false) {}

    /// Initialize with a range of 10
    AttachmentBehaviour(Entity &parent, float attachment, float clinginess, float unattachment)
        : AttachmentBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick() override;
};
