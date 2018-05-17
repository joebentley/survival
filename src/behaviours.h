#ifndef BEHAVIOURS_H_
#define BEHAVIOURS_H_

#include "entity.h"
#include "utils.h"

// AI behaviours

struct WanderBehaviour : Behaviour {
    explicit WanderBehaviour(Entity& parent) : Behaviour("WanderBehaviour", parent) {}
    void tick() override;
};

struct AttachmentBehaviour : Behaviour {
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

struct WanderAttachBehaviour : Behaviour {
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

struct ChaseAndAttackBehaviour : Behaviour {
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

struct HostilityBehaviour : Behaviour {
    HostilityBehaviour(Entity& parent, float range, float hostility)
        : Behaviour("HostilityBehaviour", parent), range(range), hostility(hostility) {}

    float range;
    float hostility;
    void tick() override;
};

// Behaviours for items

struct EatableBehaviour : Behaviour {
    EatableBehaviour(Entity& parent, float hungerRestoration)
            : Behaviour("EatableBehaviour", parent), hungerRestoration(hungerRestoration) {}

    float hungerRestoration;
};

struct ApplyableBehaviour : Behaviour {
    explicit ApplyableBehaviour(Entity& parent)
            : Behaviour("ApplyableBehaviour", parent) {}

    virtual void apply() {};
};

struct HealingItemBehaviour : ApplyableBehaviour {
    HealingItemBehaviour(Entity& parent, float healingAmount)
            : ApplyableBehaviour(parent), healingAmount(healingAmount) {}

    float healingAmount;

    void apply() override {
        auto player = EntityManager::getInstance().getEntityByID("Player");
        player->addHealth(healingAmount);
        player->removeFromInventory(parent.ID);
        parent.destroy();
    }
};

struct PickuppableBehaviour : Behaviour {
    PickuppableBehaviour(Entity& parent, int weight)
            : Behaviour("PickuppableBehaviour", parent), weight(weight) {}

    int weight;
};

struct CraftingMaterialBehaviour : Behaviour {
    CraftingMaterialBehaviour(Entity& parent, std::string materialType, float materialQuality)
            : Behaviour("CraftingMaterialBehaviour", parent), type(std::move(materialType)), quality(materialQuality) {}

    std::string type;
    float quality;
};

struct MeleeWeaponBehaviour : Behaviour {
    MeleeWeaponBehaviour(Entity &parent, int extraDamage)
            : Behaviour("MeleeWeaponBehaviour", parent), extraDamage(extraDamage) {}

    int extraDamage;
};

// WARNING: you have to add the initial item in your Entities' constructor, or otherwise you could get a huge number
// of entities being added by tick() in a single game tick
// Also, T must be an Entity that has a constructor with no arguments
template<typename T>
class KeepStockedBehaviour : public Behaviour {
    const int restockRate;
    int ticksUntilRestock;

public:
    explicit KeepStockedBehaviour(Entity& parent, int restockRate)
            : Behaviour("KeepStockedBehaviour", parent), restockRate(restockRate), ticksUntilRestock(restockRate) {}

    void tick() override {
        if (ticksUntilRestock == 0 && parent.isInventoryEmpty()) {
            auto item = std::make_shared<T>();
            parent.addToInventory(item);
            ticksUntilRestock = restockRate;
        }
        if (ticksUntilRestock > 0)
            --ticksUntilRestock;
    }
};

// Misc behaviours

struct LightEmittingBehaviour : Behaviour {
    LightEmittingBehaviour(Entity& parent, int radius)
            : Behaviour("LightEmittingBehaviour", parent), radius(radius) {}

    int getRadius() const {
        return radius;
    }

    void setRadius(int radius) {
        LightEmittingBehaviour::radius = radius;
    }

    bool isEnabled() const override {
        // Don't be enabled if just sitting in someone's inventory
        return !parent.isInAnInventory || parent.isEquipped;
    }

private:
    int radius;
};

struct EquippableBehaviour : Behaviour {
    EquippableBehaviour(Entity& parent, std::vector<EquipmentSlot> equippableSlots)
        : Behaviour("EquippableBehaviour", parent), equippableSlots(std::move(equippableSlots)) {}

    EquippableBehaviour(Entity& parent, EquipmentSlot equippableSlot)
        : EquippableBehaviour(parent, std::vector<EquipmentSlot>(1, equippableSlot)) {}

    const std::vector<EquipmentSlot> &getEquippableSlots() const {
        return equippableSlots;
    }

    bool isEquippableInSlot(EquipmentSlot slot) const {
        return std::find(equippableSlots.cbegin(), equippableSlots.cend(), slot) != equippableSlots.cend();
    };

private:
    std::vector<EquipmentSlot> equippableSlots;
};

#endif // BEHAVIOURS_H_