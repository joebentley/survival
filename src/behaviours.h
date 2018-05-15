#ifndef BEHAVIOURS_H_
#define BEHAVIOURS_H_

#include "entity.h"
#include "utils.h"

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
    bool onlyWander {false};

    WanderAttachBehaviour(Entity& parent, float attachment, float clinginess, float unattachment, float range)
            : Behaviour("WanderAttachBehaviour", parent), wander(parent), attach(parent, attachment, clinginess, unattachment, range) {}

    WanderAttachBehaviour(Entity& parent, float attachment, float clinginess, float unattachment)
            : WanderAttachBehaviour(parent, attachment, clinginess, unattachment, 10) {}

    void tick() override {
        if (onlyWander) {
            wander.tick();
            return;
        }

        float r = randFloat();

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

class ChaseAndAttackBehaviour : public Behaviour {
public:
    explicit ChaseAndAttackBehaviour(Entity& parent, float clinginess, float unattachment, float range, float postHostilityRange, float postHostility)
            : Behaviour("ChaseAndAttackBehaviour", parent), clinginess(clinginess), unattachment(unattachment),
              range(range), postHostilityRange(postHostilityRange), postHostility(postHostility) {}

    float clinginess;
    float unattachment;
    float range;
    float postHostilityRange;
    float postHostility;
    void tick() override;
};

class HostilityBehaviour : public Behaviour {
public:
    HostilityBehaviour(Entity& parent, float range, float hostility)
        : Behaviour("HostilityBehaviour", parent), range(range), hostility(hostility) {}

    float range;
    float hostility;
    void tick() override;
};

class EatableBehaviour : public Behaviour {
public:
    EatableBehaviour(Entity& parent, float hungerRestoration)
            : Behaviour("EatableBehaviour", parent), hungerRestoration(hungerRestoration) {}

    double hungerRestoration;
};

class PickuppableBehaviour : public Behaviour {
public:
    PickuppableBehaviour(Entity& parent, int weight)
            : Behaviour("PickuppableBehaviour", parent), weight(weight) {}

    int weight;
};

// T must be an Entity that takes one constructor parameter, being the entity ID
template<typename T>
class KeepStockedBehaviour : public Behaviour {
    const int restockRate;
    int ticksUntilRestock;
    int numTimesRestocked {0};

public:
    explicit KeepStockedBehaviour(Entity& parent, int restockRate)
            : Behaviour("KeepStockedBehaviour", parent), restockRate(restockRate), ticksUntilRestock(restockRate) {}

    void initialize() override {
        auto item = std::make_shared<T>(parent.ID + "berry" + std::to_string(++numTimesRestocked));
//        EntityManager::getInstance().addEntity(item);
        parent.addToInventory(std::dynamic_pointer_cast<Entity>(item));
    }

    void tick() override {
        if (ticksUntilRestock == 0 && parent.inventory.empty()) {
            auto item = std::make_shared<T>(parent.ID + "berry" + std::to_string(++numTimesRestocked));
//            EntityManager::getInstance().addEntity(item);
            parent.addToInventory(std::dynamic_pointer_cast<Entity>(item));
            ticksUntilRestock = restockRate;
        }
        if (ticksUntilRestock > 0)
            --ticksUntilRestock;
    }
};

#endif // BEHAVIOURS_H_