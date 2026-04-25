#include "Entity.h"
#include "../Behaviour/Behaviour.h"
#include "../Font.h"
#include "../Point.h"
#include "../Property/Properties/AdditionalCarryWeightProperty.h"
#include "../Property/Properties/CraftingMaterialProperty.h"
#include "../Property/Properties/EquippableProperty.h"
#include "../Property/Properties/MeleeWeaponDamageProperty.h"
#include "../Property/Properties/PickuppableProperty.h"
#include "../World.h"
#include "EntityManager.h"

#include <iostream>
#include <stdexcept>

int gNumInitialisedEntities = 0;

Entity::Entity(std::string ID, std::string name, std::string graphic, float hp, float maxhp, float regenPerTick,
               int hitTimes, int hitAmount, int maxCarryWeight)
    : mHp(hp), mMaxHp(maxhp), mRegenPerTick(regenPerTick), mHitTimes(hitTimes), mHitAmount(hitAmount),
      mID(std::move(ID)), mName(std::move(name)), mGraphic(std::move(graphic)), mPos(0, 0),
      mMaxCarryWeight(maxCarryWeight) {
    // If we specify an empty string generate a random ID
    if (this->mID.empty())
        this->mID = std::to_string(rand());
    // Add 1 to number of existing entities
    gNumInitialisedEntities++;

    // Add all equipment slots
    mEquipment[EquipmentSlot::HEAD] = "";
    mEquipment[EquipmentSlot::TORSO] = "";
    mEquipment[EquipmentSlot::LEGS] = "";
    mEquipment[EquipmentSlot::RIGHT_HAND] = "";
    mEquipment[EquipmentSlot::LEFT_HAND] = "";
    mEquipment[EquipmentSlot::FEET] = "";
    mEquipment[EquipmentSlot::BACK] = "";
}

void Entity::addBehaviour(std::unique_ptr<Behaviour> behaviour) { mBehaviours[behaviour->mID] = std::move(behaviour); }

void Entity::tick() {
    if (mHp < mMaxHp)
        mHp += mRegenPerTick;

    if (mHp > mMaxHp)
        mHp = mMaxHp;

    for (auto &behaviour : mBehaviours) {
        if (behaviour.second->isEnabled())
            behaviour.second->tick();
    }
}

void Entity::emit(Uint32 signal) {
    for (auto &behaviour : mBehaviours) {
        behaviour.second->handle(signal);
    }
}

void Entity::render(Font &font, Point currentWorldPos) {
    if (!mShouldRender)
        return;

    Point screenPos = World::worldToScreen(mPos);

    // Only draw if the entity is on the current world screen
    if (isOnScreen(currentWorldPos)) {
        font.drawText(mGraphic, screenPos.mX, screenPos.mY);
    }
}

bool Entity::isOnScreen(const Point &currentWorldPos) {
    int currentWorldX = currentWorldPos.mX;
    int currentWorldY = currentWorldPos.mY;

    return mPos.mX >= World::SCREEN_WIDTH * currentWorldX && mPos.mX < World::SCREEN_WIDTH * (currentWorldX + 1) &&
           mPos.mY >= World::SCREEN_HEIGHT * currentWorldY && mPos.mY < World::SCREEN_HEIGHT * (currentWorldY + 1);
}

bool Entity::collide(const Point &pos) { return mIsSolid && mPos == pos; }

Behaviour *Entity::getBehaviourByID(const std::string &ID) const {
    if (mBehaviours.find(ID) == mBehaviours.cend())
        return nullptr;
    return mBehaviours.at(ID).get();
}

int Entity::computeMaxDamage() const {
    if (hasEquippedInSlot(EquipmentSlot::RIGHT_HAND)) {
        auto a = EntityManager::getInstance().getEntityByID(getEquipmentID(EquipmentSlot::RIGHT_HAND));
        auto b = a->getProperty<MeleeWeaponDamageProperty>();

        if (b != nullptr) {
            return mHitAmount + b->damage;
        }
    }

    return mHitAmount;
}

int Entity::rollDamage() {
    int totalDamage = 0;
    for (int i = 0; i < mHitTimes; ++i) {
        totalDamage += rand() % (computeMaxDamage() + 1);
    }
    return totalDamage;
}

bool Entity::addToInventory(const std::string &ID) {
    auto item = EntityManager::getInstance().getEntityByID(ID);

    if (item == nullptr)
        throw std::invalid_argument("ID " + ID + " not found in Entity Manager");

    auto b = item->getProperty<PickuppableProperty>();
    if (b != nullptr) {
        if (getCarryingWeight() + b->weight > getMaxCarryWeight())
            return false;
        item->setPos(mPos);
        mInventory.push_back(item->mID);
        item->mShouldRender = false;
        item->mIsInAnInventory = true;
        return true;
    } else {
        throw std::invalid_argument("item does not have Pickuppable property");
    }
}

void Entity::removeFromInventory(const std::string &ID) {
    mInventory.erase(std::remove(mInventory.begin(), mInventory.end(), ID), mInventory.end());
}

void Entity::removeFromInventory(int inventoryIndex) { mInventory.erase(mInventory.begin() + inventoryIndex); }

void Entity::dropItem(int inventoryIndex) {
    auto item = EntityManager::getInstance().getEntityByID(mInventory[inventoryIndex]);

    mInventory.erase(mInventory.begin() + inventoryIndex);
    item->mShouldRender = true;
    item->mIsInAnInventory = false;
    item->setPos(mPos);
}

Entity *Entity::getInventoryItem(int inventoryIndex) const {
    return EntityManager::getInstance().getEntityByID(mInventory[inventoryIndex]);
}

size_t Entity::getInventorySize() const { return mInventory.size(); }

bool Entity::isInventoryEmpty() const { return mInventory.empty(); }

bool Entity::hasBehaviour(const std::string &ID) const { return mBehaviours.find(ID) != mBehaviours.end(); }

void Entity::disableWanderBehaviours() {
    // Disable wandering and wanderattach
    auto b = getBehaviourByID("WanderBehaviour");
    if (b != nullptr)
        b->disable();
    b = getBehaviourByID("WanderAttachBehaviour");
    if (b != nullptr)
        b->disable();
}

void Entity::enableWanderBehaviours() {
    // Enable wandering and wanderattach
    auto b = getBehaviourByID("WanderBehaviour");
    if (b != nullptr)
        b->enable();
    b = getBehaviourByID("WanderAttachBehaviour");
    if (b != nullptr)
        b->enable();
}

void Entity::destroy() { EntityManager::getInstance().eraseByID(mID); }

int Entity::getCarryingWeight() {
    int totalWeight = 0;
    for (const auto &ID : mInventory) {
        auto item = EntityManager::getInstance().getEntityByID(ID);
        auto pickuppable = item->getProperty<PickuppableProperty>();
        if (pickuppable != nullptr) {
            totalWeight += pickuppable->weight;
        }
    }
    return totalWeight;
}

void Entity::addHealth(float health) { this->mHp = std::min(this->mHp + health, mMaxHp); }

std::vector<Entity *> Entity::getInventory() const {
    std::vector<Entity *> output;

    std::transform(mInventory.cbegin(), mInventory.cend(), std::back_inserter(output),
                   [](auto &a) -> Entity * { return EntityManager::getInstance().getEntityByID(a); });

    return output;
}

bool Entity::isInInventory(const std::string &ID) const {
    return std::find(mInventory.cbegin(), mInventory.cend(), ID) != mInventory.cend();
}

bool Entity::moveTo(Point p) {
    auto &em = EntityManager::getInstance();
    std::vector<Entity *> entities = em.doCollisions(p, *this);

    // Check that there were no collisions in the space
    if (entities.empty()) {
        Point oldWorldPos = getWorldPos();
        setPos(p);
        // Check if we moved to a new world coordinate, if so update the current entities on screen
        if (oldWorldPos != getWorldPos())
            em.recomputeCurrentEntitiesOnScreenAndSurroundingScreens();

        // Also move all items held by entity
        for (const std::string &ID : mInventory)
            em.getEntityByID(ID)->setPos(p);

        return true;
    }
    return false;
}

const std::unordered_map<EquipmentSlot, std::string> &Entity::getEquipment() const { return mEquipment; }

bool Entity::equip(EquipmentSlot slot, Entity *entity) {
    auto equippable = entity->getProperty<EquippableProperty>();
    if (entity->hasProperty("Pickuppable") && equippable != nullptr) {
        if (equippable->isEquippableInSlot(slot)) {
            // Make sure it is in the player inventory (and in turn the entity manager)
            if (!isInInventory(entity->mID))
                Entity::addToInventory(entity->mID);

            entity->mIsEquipped = true;
            mEquipment[slot] = entity->mID;
            return true;
        }
    }
    return false;
}

bool Entity::equip(EquipmentSlot slot, const std::string &ID) {
    return equip(slot, EntityManager::getInstance().getEntityByID(ID));
}

bool Entity::unequip(Entity *entity) { return unequip(entity->mID); }

bool Entity::unequip(const std::string &ID) {
    auto a = std::find_if(mEquipment.cbegin(), mEquipment.cend(), [ID](auto b) { return b.second == ID; });

    if (a == mEquipment.cend())
        return false;
    else {
        EntityManager::getInstance().getEntityByID(a->second)->mIsEquipped = false;
        mEquipment[a->first] = "";
        return true;
    }
}

bool Entity::unequip(EquipmentSlot slot) {
    if (mEquipment[slot].empty())
        return false;

    EntityManager::getInstance().getEntityByID(getEquipmentID(slot))->mIsEquipped = false;
    mEquipment[slot] = "";
    return true;
}

Entity *Entity::getEquipmentEntity(EquipmentSlot slot) const {
    if (mEquipment.find(slot) == mEquipment.cend())
        return nullptr;
    return EntityManager::getInstance().getEntityByID(mEquipment.at(slot));
}

std::vector<std::string> Entity::getInventoryItemsEquippableInSlot(EquipmentSlot slot) const {
    std::vector<std::string> IDs;

    std::copy_if(mInventory.cbegin(), mInventory.cend(), std::back_inserter(IDs), [slot](const std::string &ID) {
        auto e = EntityManager::getInstance().getEntityByID(ID);
        auto equippable = e->getProperty<EquippableProperty>();
        if (!e->mIsEquipped && equippable != nullptr) {
            return equippable->isEquippableInSlot(slot);
        }
        return false;
    });

    return IDs;
}

std::string Entity::getEquipmentID(EquipmentSlot slot) const { return mEquipment.at(slot); }

bool Entity::hasEquippedInSlot(EquipmentSlot slot) const { return !mEquipment.at(slot).empty(); }

bool Entity::hasEquipped(const std::string &ID) const {
    return std::find_if(mEquipment.cbegin(), mEquipment.cend(), [ID](auto &a) { return a.second == ID; }) !=
           mEquipment.cend();
}

EquipmentSlot Entity::getEquipmentSlotByID(const std::string &ID) const {
    auto a = std::find_if(mEquipment.cbegin(), mEquipment.cend(), [ID](auto &a) { return a.second == ID; });

    if (a == mEquipment.cend())
        throw std::out_of_range("Nothing equipped with ID: " + ID);

    return a->first;
}

std::string Entity::getInventoryItemID(int inventoryIndex) const { return mInventory[inventoryIndex]; }

int Entity::getMaxCarryWeight() const {
    auto a = getEquipmentEntity(EquipmentSlot::BACK);

    if (a != nullptr) {
        auto b = a->getProperty<AdditionalCarryWeightProperty>();
        if (b != nullptr) {
            return mMaxCarryWeight + b->additionalCarryWeight;
        }
    }

    return mMaxCarryWeight;
}

inline std::vector<std::string> Entity::filterInventoryForCraftingMaterial(std::string materialType) const {
    return filterInventoryForCraftingMaterials(std::vector<std::string>{std::move(materialType)});
}

std::vector<std::string>
Entity::filterInventoryForCraftingMaterials(const std::vector<std::string> &materialTypes) const {
    std::vector<std::string> materialIDs;

    std::copy_if(mInventory.cbegin(), mInventory.cend(), std::back_inserter(materialIDs),
                 [materialTypes](const std::string &ID) {
                     auto entity = EntityManager::getInstance().getEntityByID(ID);
                     auto b = entity->getProperty<CraftingMaterialProperty>();
                     if (b != nullptr) {
                         if (std::find(materialTypes.cbegin(), materialTypes.cend(), b->type) != materialTypes.cend()) {
                             return true;
                         }
                     }
                     return false;
                 });

    return materialIDs;
}

bool Entity::hasProperty(const std::string &propertyName) const {
    return mProperties.find(propertyName) != mProperties.cend();
}

Property *Entity::getProperty(const std::string &propertyName) const {
    if (mProperties.find(propertyName) == mProperties.cend())
        return nullptr;
    return mProperties.at(propertyName).get();
}

void Entity::addProperty(std::unique_ptr<Property> property) { mProperties[property->getName()] = std::move(property); }

void Entity::setPos(int x, int y) { setPos(Point(x, y)); }

void Entity::setPos(Point p) { mPos = p; }

Point Entity::getPos() const { return mPos; }

Point Entity::getWorldPos() const {
    return {this->mPos.mX / World::SCREEN_WIDTH, this->mPos.mY / World::SCREEN_HEIGHT};
}