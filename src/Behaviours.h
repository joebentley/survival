#ifndef BEHAVIOURS_H_
#define BEHAVIOURS_H_

#include "Font.h"
#include "Entity.h"
#include "utils.h"

//region AI behaviours

/// This behaviour causes the parent entity to wander aimlessly in every direction
struct WanderBehaviour : Behaviour {
    explicit WanderBehaviour(Entity& parent) : Behaviour("WanderBehaviour", parent) {}
    void tick() override;
};

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
    AttachmentBehaviour(Entity& parent, float attachment, float clinginess, float unattachment, float range)
        : Behaviour("AttachmentBehaviour", parent),
          attachment(attachment), clinginess(clinginess), unattachment(unattachment), range(range), attached(false) {}

    /// Initialize with a range of 10
    AttachmentBehaviour(Entity& parent, float attachment, float clinginess, float unattachment)
        : AttachmentBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick() override;
};

/// Combination of a WanderBehaviour and AttachBehaviour, with a random probability to go from wander to attach
struct WanderAttachBehaviour : Behaviour {
    WanderBehaviour wander;
    AttachmentBehaviour attach;
    /// Only wander, don't attach
    bool onlyWander {false};

    /// Initialize the behaviour, giving parameters for the AttachBehaviour
    /// \param parent parent of this entity
    /// \param attachment probability of attaching to player
    /// \param clinginess probability of moving to player when attached
    /// \param unattachment probability of unattaching from player
    /// \param range how close we need to be to attach
    WanderAttachBehaviour(Entity& parent, float attachment, float clinginess, float unattachment, float range)
            : Behaviour("WanderAttachBehaviour", parent), wander(parent), attach(parent, attachment, clinginess, unattachment, range) {}

    /// Initialize with a range of 10
    WanderAttachBehaviour(Entity& parent, float attachment, float clinginess, float unattachment)
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

/// Seek out a Home entity (identified by given name) if nearby and hole up within it
struct SeekHomeBehaviour : Behaviour {
    /// Name of home entities to go to
    std::string homeName;
    /// Range within which to start moving to the home entity
    float range;
    /// Probability to choose a home to start going towards
    float homeAttachmentProbability;
    /// Probability to leave the home
    float homeFlightProbability;

    explicit SeekHomeBehaviour(Entity& parent, std::string homeName, float range = 20, float homeAttachmentProbability = 0.1,
                               float homeFlightProbability = 0.1)
        : Behaviour("SeekHomeBehaviour", parent), homeName(std::move(homeName)), range(range),
            homeAttachmentProbability(homeAttachmentProbability), homeFlightProbability(homeFlightProbability) {}

    void tick() override;

private:
    /// ID of current home target
    std::string homeTargetID;
};

/// Chase and attack the entity with ID "Player". If parent has a "WanderBehaviour" then that will enable after attacking,
/// if it has a "WanderAttachBehaviour" it will wander but not attach anymore, if it has "HostilityBehaviour" then re-enable that,
/// otherwise if postHostility != 0 it will create a new "HostilityBehaviour" with parameters postHostilityRange and postHostility
struct ChaseAndAttackBehaviour : Behaviour {
    /// Initialize the behaviour
    /// \param parent parent of this behaviour
    /// \param clinginess probability of moving towards the player on tick
    /// \param unattachment probability to stop attacking if out of range
    /// \param range distance at which we might stop attacking and chasing player
    /// \param postHostilityRange range at which to be hostile to player within after stopping attacking
    /// \param postHostility probability of becoming hostile after stopping attacking
    ChaseAndAttackBehaviour(Entity& parent, float clinginess, float unattachment, float range, float postHostilityRange, float postHostility)
            : Behaviour("ChaseAndAttackBehaviour", parent), clinginess(clinginess), unattachment(unattachment),
              range(range), postHostilityRange(postHostilityRange), postHostility(postHostility) {}

    float clinginess;
    float unattachment;
    float range;
    float postHostilityRange;
    float postHostility;
    void tick() override;
};

/// Chase and attack the player if in range
struct HostilityBehaviour : Behaviour {
    /// Initialize the behaviour. Will throw exception if entity has no "ChaseAndAttackBehaviour"
    /// \param parent parent entity of this behaviour
    /// \param range range in which to consider attacking
    /// \param hostility probability to start attacking
    HostilityBehaviour(Entity& parent, float range, float hostility)
        : Behaviour("HostilityBehaviour", parent), range(range), hostility(hostility)
    {
        if (!parent.hasBehaviour("ChaseAndAttackBehaviour"))
            throw std::invalid_argument("Error: HostilityBehaviour cannot be added to entity with ID " + parent.mID
                                        + " as it does not have a ChaseAndAttackBehaviour");
    }

    float range;
    float hostility; // in [0, 1]
    void tick() override;
};

//endregion

//region Behaviours for items

// TODO: This should be a property, but difficult due to virtual methods
/// Abstract base class to represent behaviours that have an apply method, for example items that can be used
struct ApplyableBehaviour : Behaviour {
    ApplyableBehaviour(std::string ID, Entity& parent)
            : Behaviour(std::move(ID), parent) {}

    virtual void apply() = 0;
};

// TODO: enemies should be able to use it to heal themselves
/// Represents an item that heals the player, not exceeding their maximum health
struct HealingItemBehaviour : ApplyableBehaviour {
    /// Initialize the behaviour
    /// \param parent parent entity
    /// \param healingAmount amount to heal the player
    HealingItemBehaviour(Entity& parent, float healingAmount)
            : ApplyableBehaviour("HealingItemBehaviour", parent), healingAmount(healingAmount) {}

    float healingAmount;

    void apply() override {
        auto player = EntityManager::getInstance().getEntityByID("Player");
        player->addHealth(healingAmount);
        // destroy the parent entity once it is used
        player->removeFromInventory(mParent.mID);
        mParent.destroy();
    }
};

/// Keep the parent entity's inventory stocked with the entity given by T
/// WARNING: you should add the initial item in your Entities' constructor, or otherwise you could get a huge number
/// of entities being added by tick() in a single game tick for all the different entities with "KeepStockedBehaviour"
/// Also, T must be an Entity that has a constructor with no arguments
template<typename T>
class KeepStockedBehaviour : public Behaviour {
    const int restockRate;
    int ticksUntilRestock;

public:
    KeepStockedBehaviour(Entity& parent, int restockRate)
            : Behaviour("KeepStockedBehaviour", parent), restockRate(restockRate), ticksUntilRestock(restockRate) {}

    void tick() override {
        if (ticksUntilRestock == 0 && mParent.isInventoryEmpty()) {
            auto item = std::make_unique<T>();
            auto ID = item->mID;
            EntityManager::getInstance().addEntity(std::move(item));
            mParent.addToInventory(ID);
            ticksUntilRestock = restockRate;
        }
        if (ticksUntilRestock > 0)
            --ticksUntilRestock;
    }
};

//endregion

//region Misc behaviours

// TODO: This should be a property, but difficult due to virtual methods
/// Represents an entity that can be interacted with by the player, and can hijack input handling and rendering
struct InteractableBehaviour : Behaviour {
    explicit InteractableBehaviour(Entity &parent)
            : Behaviour("InteractableBehaviour", parent) {}

    /// Handle input from the player entity. If it returns false then the interaction with the player will end.
    /// \param e the raw SDL keyboard event
    /// \return if false, end the current interaction
    virtual bool handleInput(SDL_KeyboardEvent &) { return false; }

    /// Allows custom rendering to be done by the behaviour if required
    virtual void render(Font &) {}
};

//endregion

#endif // BEHAVIOURS_H_
