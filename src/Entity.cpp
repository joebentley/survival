#include "Entity.h"
#include "Behaviours.h"
#include "Properties.h"

#include <stdexcept>
#include <cmath>
#include <any>

int gNumInitialisedEntities = 0;

void Entity::addBehaviour(std::unique_ptr<Behaviour> behaviour) {
    mBehaviours[behaviour->mID] = std::move(behaviour);
}

void Entity::tick() {
    if (mHp < mMaxHp)
        mHp += mRegenPerTick;

    if (mHp > mMaxHp)
        mHp = mMaxHp;

    for (auto& behaviour : mBehaviours) {
        if (behaviour.second->isEnabled())
            behaviour.second->tick();
    }
}

void Entity::emit(Uint32 signal) {
    for (auto& behaviour : mBehaviours) {
        behaviour.second->handle(signal);
    }
}

void Entity::render(Font& font, Point currentWorldPos) {
    if (!mShouldRender)
        return;

    Point screenPos = worldToScreen(mPos);

    // Only draw if the entity is on the current world screen
    if (isOnScreen(currentWorldPos)) {
        font.drawText(mGraphic, screenPos.mX, screenPos.mY);
    }
}

bool Entity::isOnScreen(const Point &currentWorldPos) {
    int currentWorldX = currentWorldPos.mX;
    int currentWorldY = currentWorldPos.mY;

    return mPos.mX >= SCREEN_WIDTH * currentWorldX && mPos.mX < SCREEN_WIDTH * (currentWorldX + 1) &&
           mPos.mY >= SCREEN_HEIGHT * currentWorldY && mPos.mY < SCREEN_HEIGHT * (currentWorldY + 1);
}

bool Entity::collide(const Point &pos) {
    return mIsSolid && mPos == pos;
}

Behaviour *Entity::getBehaviourByID(const std::string &ID) const {
    if (mBehaviours.find(ID) == mBehaviours.cend())
        return nullptr;
    return mBehaviours.at(ID).get();
}


int Entity::computeMaxDamage() const {
    if (hasEquippedInSlot(EquipmentSlot::RIGHT_HAND)) {
        auto a = EntityManager::getInstance().getEntityByID(getEquipmentID(EquipmentSlot::RIGHT_HAND));
        auto b = a->getProperty("MeleeWeaponDamage");

        if (b != nullptr) {
            return mHitAmount + std::any_cast<int>(b->getValue());
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

    auto b = item->getProperty("Pickuppable");
    if (b != nullptr) {
        if (getCarryingWeight() + std::any_cast<int>(b->getValue()) > getMaxCarryWeight())
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

void Entity::removeFromInventory(int inventoryIndex) {
    mInventory.erase(mInventory.begin() + inventoryIndex);
}

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


size_t Entity::getInventorySize() const {
    return mInventory.size();
}

bool Entity::isInventoryEmpty() const {
    return mInventory.empty();
}

bool Entity::hasBehaviour(const std::string &ID) const {
    return mBehaviours.find(ID) != mBehaviours.end();
}

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

void Entity::destroy() {
    EntityManager::getInstance().eraseByID(mID);
}

int Entity::getCarryingWeight() {
    int totalWeight = 0;
    for (const auto& ID : mInventory) {
        auto item = EntityManager::getInstance().getEntityByID(ID);
        auto pickuppable = item->getProperty("Pickuppable");
        if (pickuppable != nullptr) {
            totalWeight += std::any_cast<int>(pickuppable->getValue());
        }
    }
    return totalWeight;
}

void Entity::addHealth(float health) {
    this->mHp = std::min(this->mHp + health, mMaxHp);
}

std::vector<Entity *> Entity::getInventory() const {
    std::vector<Entity *> output;

    std::transform(mInventory.cbegin(), mInventory.cend(), std::back_inserter(output),
    [] (auto &a) -> Entity * {
        return EntityManager::getInstance().getEntityByID(a);
    });

    return output;
}

bool Entity::isInInventory(const std::string &ID) const {
    return std::find(mInventory.cbegin(), mInventory.cend(), ID) != mInventory.cend();
}

bool Entity::moveTo(Point p) {
    auto &em = EntityManager::getInstance();
    std::vector<Entity*> entities = em.doCollisions(p, *this);

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

const std::unordered_map<EquipmentSlot, std::string> &Entity::getEquipment() const {
    return mEquipment;
}

bool Entity::equip(EquipmentSlot slot, Entity *entity) {
    auto equippable = entity->getProperty("Equippable");
    if (entity->hasProperty("Pickuppable") && equippable != nullptr) {
        auto b = std::any_cast<EquippableProperty::Equippable>(equippable->getValue());
        if (b.isEquippableInSlot(slot)) {
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

bool Entity::unequip(Entity *entity) {
    return unequip(entity->mID);
}

bool Entity::unequip(const std::string &ID) {
    auto a = std::find_if(mEquipment.cbegin(), mEquipment.cend(), [ID] (auto b) { return b.second == ID; });

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

    std::copy_if(mInventory.cbegin(), mInventory.cend(), std::back_inserter(IDs), [slot] (auto &ID)
    {
        auto e = EntityManager::getInstance().getEntityByID(ID);
        auto equippable = e->getProperty("Equippable");
        if (!e->mIsEquipped && equippable != nullptr) {
            auto b = std::any_cast<EquippableProperty::Equippable>(equippable->getValue());
            return b.isEquippableInSlot(slot);
        }
        return false;
    });

    return IDs;
}

std::string Entity::getEquipmentID(EquipmentSlot slot) const {
    return mEquipment.at(slot);
}

bool Entity::hasEquippedInSlot(EquipmentSlot slot) const {
    return !mEquipment.at(slot).empty();
}

bool Entity::hasEquipped(const std::string &ID) const {
    return std::find_if(mEquipment.cbegin(), mEquipment.cend(), [ID] (auto &a) { return a.second == ID; }) != mEquipment.cend();
}

EquipmentSlot Entity::getEquipmentSlotByID(const std::string &ID) const {
    auto a = std::find_if(mEquipment.cbegin(), mEquipment.cend(), [ID] (auto &a) { return a.second == ID; });

    if (a == mEquipment.cend())
        throw std::out_of_range("Nothing equipped with ID: " + ID);

    return a->first;
}

std::string Entity::getInventoryItemID(int inventoryIndex) const {
    return mInventory[inventoryIndex];
}

int Entity::getMaxCarryWeight() const {
    auto a = getEquipmentEntity(EquipmentSlot::BACK);

    if (a != nullptr) {
        auto b = a->getProperty("AdditionalCarryWeight");
        if (b != nullptr) {
            return mMaxCarryWeight + std::any_cast<int>(b->getValue());
        }
    }

    return mMaxCarryWeight;
}

inline std::vector<std::string> Entity::filterInventoryForCraftingMaterial(std::string materialType) const {
    return filterInventoryForCraftingMaterials(std::vector<std::string> {std::move(materialType)});
}

std::vector<std::string> Entity::filterInventoryForCraftingMaterials(const std::vector<std::string> &materialTypes) const {
    std::vector<std::string> materialIDs;

    std::copy_if(mInventory.cbegin(), mInventory.cend(), std::back_inserter(materialIDs),
    [materialTypes] (const auto &ID) {
        auto entity = EntityManager::getInstance().getEntityByID(ID);
        auto b = entity->getProperty("CraftingMaterial");
        if (b != nullptr) {
            if (std::find(materialTypes.cbegin(), materialTypes.cend(),
                    std::any_cast<CraftingMaterialProperty::Data>(b->getValue()).type) != materialTypes.cend()) {
                return true;
            }
        }
        return false;
    });

    return materialIDs;
}

bool Entity::hasProperty(const std::string &propertyName) const {
    if (!PropertiesManager::getInstance().isPropertyRegistered(propertyName))
        throw std::out_of_range("Property " + propertyName + " not registered into the list of known properties!");

    return mProperties.find(propertyName) != mProperties.cend();
}

Property *Entity::getProperty(const std::string &propertyName) const {
    if (!PropertiesManager::getInstance().isPropertyRegistered(propertyName))
        throw std::out_of_range("Property " + propertyName + " not registered into the list of known properties!");

    if (mProperties.find(propertyName) == mProperties.cend())
        return nullptr;
    return mProperties.at(propertyName).get();
}

void Entity::addProperty(std::unique_ptr<Property> property) {
    mProperties[property->getName()] = std::move(property);
}

void EntityManager::addEntity(std::unique_ptr<Entity> entity) {
    if (getEntityByID(entity->mID) != nullptr)
        throw std::invalid_argument("Entity with ID " + entity->mID + " already present!");

    mEntities[entity->mID] = std::move(entity);

    // Make sure new entities trigger a refresh of the render order list and entity caches
    if (!mToRender.empty())
        recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::broadcast(Uint32 signal) {
    for (const auto& entity : mEntities) {
        entity.second->emit(signal);
    }
}

#define UNMANAGED_ENTITIES_ERROR_MESSAGE "Warning! The number of initialised entities is not equal to the number of entities in the entity manager!"

void EntityManager::initialize() {
    if ((size_t)gNumInitialisedEntities != mEntities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::tick() {
    cleanup();

    mTimeOfDay += mTimePerTick;

    if ((size_t)gNumInitialisedEntities != mEntities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    // Only update entities in this screen and surrounding screens
    // TODO: add special type of entity that always keeps updated e.g. the player's farm
    for (const auto& ID : mCurrentlyOnScreen)
        getEntityByID(ID)->tick();
    for (const auto& ID : mInSurroundingScreens)
        getEntityByID(ID)->tick();
}

void EntityManager::render(Font &font, Point currentWorldPos, LightMapTexture &lightMapTexture) {
    for (const auto& a : mToRender) {
        getEntityByID(a.first)->render(font, currentWorldPos);
    }

    // Draw time-of-day fog
    auto frac = EntityManager::getInstance().getTimeOfDay().getFractionOfDay();
    auto a = 0.6 + 0.8 * std::sin(2 * M_PI * frac - M_PI / 2);
    if (a < 0)
        a = 0;
    else if (a > 1)
        a = 1;
    auto alpha = static_cast<Uint8>(a * 0xFF);
    lightMapTexture.render(getLightSources(font.getCellSize()), alpha);
}

void EntityManager::render(Font &font, LightMapTexture &lightMapTexture) {
    render(font, getEntityByID("Player")->getWorldPos(), lightMapTexture);
}

Entity *EntityManager::getEntityByID(const std::string &ID) const {
    if (mEntities.find(ID) == mEntities.cend())
        return nullptr;
    return mEntities.at(ID).get();
}

std::vector<Entity *> EntityManager::getEntitiesAtPos(const Point &pos) const {
    std::vector<Entity *> entitiesAtPos;

    for (const auto& entity : mEntities) {
        if (entity.second->getPos() == pos)
            entitiesAtPos.push_back(entity.second.get());
    }

    return entitiesAtPos;
}

std::vector<Entity *> EntityManager::getEntitiesAtPosFaster(const Point &pos) const {
    std::vector<Entity *> entitiesAtPos;

    // TODO: remove ugly double loop
    for (const auto& ID : mInSurroundingScreens) {
        if (getEntityByID(ID)->getPos() == pos)
            entitiesAtPos.push_back(getEntityByID(ID));
    }
    for (const auto& ID : mCurrentlyOnScreen) {
        if (getEntityByID(ID)->getPos() == pos)
            entitiesAtPos.push_back(getEntityByID(ID));
    }

    return entitiesAtPos;
}

std::vector<Entity *> EntityManager::getEntitiesSurrounding(const Point &pos) const {
    std::vector<Entity *> entitiesSurrounding;
    const std::vector<Point> surroundingPoints { pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
                                                 pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

    for (const auto& entity : mEntities) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), entity.second->getPos()) != surroundingPoints.cend())
            entitiesSurrounding.emplace_back(entity.second.get());
    }

    return entitiesSurrounding;
}

std::vector<Entity *> EntityManager::getEntitiesSurroundingFaster(const Point &pos) const {
    std::vector<Entity *> entitiesSurrounding;
    const std::vector<Point> surroundingPoints { pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
                                                 pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

    // TODO: remove ugly double loop
    for (const auto& ID : mInSurroundingScreens) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), getEntityByID(ID)->getPos()) != surroundingPoints.cend())
            entitiesSurrounding.emplace_back(getEntityByID(ID));
    }
    for (const auto& ID : mCurrentlyOnScreen) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), getEntityByID(ID)->getPos()) != surroundingPoints.cend())
            entitiesSurrounding.emplace_back(getEntityByID(ID));
    }

    return entitiesSurrounding;
}

std::vector<Entity *> EntityManager::getEntitiesOnScreenAndSurroundingScreens() const {
    std::vector<Entity *> entities;
    for (const auto& ID : mCurrentlyOnScreen) {
        entities.push_back(getEntityByID(ID));
    }
    for (const auto& ID : mInSurroundingScreens) {
        entities.push_back(getEntityByID(ID));
    }
    return entities;
}

std::vector<Entity *> EntityManager::doCollisions(const Point& pos, Entity &entity) {
    std::vector<Entity *> collidingEntities;
    for (const auto& ID : mCurrentlyOnScreen) {
        Entity *e = getEntityByID(ID);
        // execute the entity's collision, which returns true if a collision occurred
        if (e->collide(pos) || e->collide(pos, entity))
            collidingEntities.push_back(e);
    }
    for (const auto& ID : mInSurroundingScreens) {
        Entity *e = getEntityByID(ID);
        if (e->collide(pos) || e->collide(pos, entity))
            collidingEntities.push_back(e);
    }
    return collidingEntities;
}

void EntityManager::cleanup() {
    while (!mToBeDeleted.empty()) {
        eraseByID(mToBeDeleted.front());
        mToBeDeleted.pop();
    }
}

void EntityManager::queueForDeletion(const std::string &ID) {
    mToBeDeleted.push(ID);
}

void EntityManager::eraseByID(const std::string &ID) {
    mEntities.erase(ID);
    --gNumInitialisedEntities;
    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::reorderEntities() {
    mToRender.clear();
    // Make pairs of entity IDs with their rendering layers
    std::transform(mCurrentlyOnScreen.cbegin(), mCurrentlyOnScreen.cend(), std::back_inserter(mToRender),
            [this] (const std::string &ID) -> auto { return std::make_pair(ID, getEntityByID(ID)->mRenderingLayer); });
    std::sort(mToRender.begin(), mToRender.end(), [](auto &a, auto &b) { return a.second > b.second; });
}

bool EntityManager::isEntityInManager(const std::string &ID) {
    return mEntities.find(ID) != mEntities.end();
}

// TODO should split this into two separate functions for current entities on screen and for surrounding screens
// player should call both current screen and surrounding screens, but other entity should only call current screen
void EntityManager::recomputeCurrentEntitiesOnScreenAndSurroundingScreens(Point currentWorldPos) {
    mCurrentlyOnScreen.clear();
    mInSurroundingScreens.clear();
    for (const auto &a : mEntities) {
        auto worldPosDiff = a.second->getWorldPos() - currentWorldPos;
        if (worldPosDiff == Point(0, 0))
            mCurrentlyOnScreen.emplace_back(a.second->mID);
        else if (std::abs(worldPosDiff.mX) <= 1 && std::abs(worldPosDiff.mY) <= 1)
            mInSurroundingScreens.emplace_back(a.second->mID);
    }
    reorderEntities();
}

const Time &EntityManager::getTimeOfDay() const {
    return mTimeOfDay;
}

void EntityManager::setTimeOfDay(const Time &timeOfDay) {
    EntityManager::mTimeOfDay = timeOfDay;
}

const Time &EntityManager::getTimePerTick() const {
    return mTimePerTick;
}

void EntityManager::setTimePerTick(const Time &timePerTick) {
    EntityManager::mTimePerTick = timePerTick;
}

std::vector<LightMapPoint> EntityManager::getLightSources(Point fontSize) const {
    std::vector<LightMapPoint> points;

    for (const auto &a : mCurrentlyOnScreen) {
        const auto &entity = getEntityByID(a);
        auto b = entity->getProperty("LightEmitting");
        if (b != nullptr) {
            auto light = std::any_cast<LightEmittingProperty::Light>(b->getValue());
            if (light.isEnabled()) {
                auto radius = fontSize.mY * light.getRadius();
                auto point = fontSize * worldToScreen(entity->getPos()) + fontSize / 2;
                points.emplace_back(LightMapPoint(point, radius, light.getColor()));
            }
        }
    }

    return points;
}

void EntityManager::recomputeCurrentEntitiesOnScreenAndSurroundingScreens() {
    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}


EquipmentSlot &operator++(EquipmentSlot &slot) {
    if (slot == EquipmentSlot::BACK) {
        slot = EquipmentSlot::HEAD;
        return slot;
    }

    slot = static_cast<EquipmentSlot>(static_cast<int>(slot) + 1);
    return slot;
}

EquipmentSlot &operator--(EquipmentSlot &slot) {
    if (slot == EquipmentSlot::HEAD) {
        slot = EquipmentSlot::BACK;
        return slot;
    }

    slot = static_cast<EquipmentSlot>(static_cast<int>(slot) - 1);
    return slot;
}
