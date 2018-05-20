#include "entity.h"
#include "behaviours.h"

#include <stdexcept>
#include <cmath>

int gNumInitialisedEntities = 0;

void Entity::addBehaviour(std::shared_ptr<Behaviour> behaviour) {
    behaviours[behaviour->ID] = std::move(behaviour);
}

void Entity::tick() {
    if (hp < maxhp)
        hp += regenPerTick;

    if (hp > maxhp)
        hp = maxhp;

    for (auto& behaviour : behaviours) {
        if (behaviour.second->isEnabled())
            behaviour.second->tick();
    }
}

void Entity::emit(Uint32 signal) {
    for (auto& behaviour : behaviours) {
        behaviour.second->handle(signal);
    }
}

void Entity::render(Font& font, Point currentWorldPos) {
    if (!shouldRender)
        return;

    int currentWorldX = currentWorldPos.x;
    int currentWorldY = currentWorldPos.y;

    // Only draw if the entity is on the current world screen
    if (
        pos.x >= SCREEN_WIDTH * currentWorldX && pos.x < SCREEN_WIDTH * (currentWorldX + 1) &&
        pos.y >= SCREEN_HEIGHT * currentWorldY && pos.y < SCREEN_HEIGHT * (currentWorldY + 1)
    )
    {
        font.drawText(graphic, this->pos.x - SCREEN_WIDTH * currentWorldX,
                               this->pos.y - SCREEN_HEIGHT * currentWorldY);
    }
}

std::shared_ptr<Behaviour> Entity::getBehaviourByID(const std::string& ID) const {
    try {
        return behaviours.at(ID);
    } catch (const std::out_of_range &e) {
        return nullptr;
    }
}


int Entity::computeMaxDamage() const {
    if (hasEquippedInSlot(EquipmentSlot::RIGHT_HAND)) {
        auto a = EntityManager::getInstance().getEntityByID(getEquipmentID(EquipmentSlot::RIGHT_HAND));

        if (a->hasBehaviour("MeleeWeaponBehaviour")) {
            auto &b = dynamic_cast<MeleeWeaponBehaviour&>(*a->getBehaviourByID("MeleeWeaponBehaviour"));
            return hitAmount + b.extraDamage;
        }
    }

    return hitAmount;
}

int Entity::rollDamage() {
    int totalDamage = 0;
    for (int i = 0; i < hitTimes; ++i) {
        totalDamage += rand() % (computeMaxDamage() + 1);
    }
    return totalDamage;
}

bool Entity::addToInventory(const std::shared_ptr<Entity> &item) {
    auto b = item->getBehaviourByID("PickuppableBehaviour");
    if (b != nullptr) {
        if (!EntityManager::getInstance().isEntityInManager(item->ID))
            EntityManager::getInstance().addEntity(item);

        if (getCarryingWeight() + dynamic_cast<PickuppableBehaviour&>(*b).weight > getMaxCarryWeight())
            return false;
        item->setPos(pos);
        inventory.push_back(item->ID);
        item->shouldRender = false;
        item->isInAnInventory = true;
        return true;
    } else {
        throw std::invalid_argument("item does not have PickuppableBehaviour");
    }
}

void Entity::removeFromInventory(const std::string &ID) {
    inventory.erase(std::remove(inventory.begin(), inventory.end(), ID), inventory.end());
}

void Entity::removeFromInventory(int inventoryIndex) {
    inventory.erase(inventory.begin() + inventoryIndex);
}

void Entity::dropItem(int inventoryIndex) {
    auto item = EntityManager::getInstance().getEntityByID(inventory[inventoryIndex]);

    inventory.erase(inventory.begin() + inventoryIndex);
    item->shouldRender = true;
    item->isInAnInventory = false;
    item->setPos(pos);
}

std::shared_ptr<Entity> Entity::getInventoryItem(int inventoryIndex) const {
    return EntityManager::getInstance().getEntityByID(inventory[inventoryIndex]);
}


size_t Entity::getInventorySize() const {
    return inventory.size();
}

bool Entity::isInventoryEmpty() const {
    return inventory.empty();
}

bool Entity::hasBehaviour(const std::string &ID) const {
    return behaviours.find(ID) != behaviours.end();
}

void Entity::destroy() {
    EntityManager::getInstance().eraseByID(ID);
}

int Entity::getCarryingWeight() {
    int totalWeight = 0;
    for (const auto& ID : inventory) {
        auto item = EntityManager::getInstance().getEntityByID(ID);
        if (item->hasBehaviour("PickuppableBehaviour")) {
            auto a = dynamic_cast<PickuppableBehaviour&>(*item->getBehaviourByID("PickuppableBehaviour"));
            totalWeight += a.weight;
        }
    }
    return totalWeight;
}

void Entity::addHealth(float health) {
    this->hp = std::min(this->hp + health, maxhp);
}

std::vector<std::shared_ptr<Entity>> Entity::getInventory() const {
    std::vector<std::shared_ptr<Entity>> output;

    std::transform(inventory.cbegin(), inventory.cend(), std::back_inserter(output),
    [] (auto &a) -> std::shared_ptr<Entity> {
        return EntityManager::getInstance().getEntityByID(a);
    });

    return output;
}

bool Entity::isInInventory(std::string ID) const {
    return std::find(inventory.cbegin(), inventory.cend(), ID) != inventory.cend();
}

Point Entity::getPos() const {
    return pos;
}

bool Entity::moveTo(Point p) {
    auto entities = EntityManager::getInstance().getEntitiesAtPosFaster(p);

    // Check if there is a solid object in space
    if (std::find_if(entities.cbegin(), entities.cend(), [] (auto &a) { return a->isSolid; }) == entities.cend()) {
        auto oldWorldPos = getWorldPos();
        setPos(p);
        // Check if we moved to a new world coordinate, if so update the current entities on screen
        if (oldWorldPos != getWorldPos())
            EntityManager::getInstance().recomputeCurrentEntitiesOnScreenAndSurroundingScreens();

        // Move all items held by entity
        for (const auto &ID : inventory)
            EntityManager::getInstance().getEntityByID(ID)->setPos(p);

        return true;
    }
    return false;
}

const std::unordered_map<EquipmentSlot, std::string> &Entity::getEquipment() const {
    return equipment;
}

bool Entity::equip(EquipmentSlot slot, std::shared_ptr<Entity> entity) {
    if (entity->hasBehaviour("EquippableBehaviour") && entity->hasBehaviour("PickuppableBehaviour")) {
        auto &b = dynamic_cast<EquippableBehaviour&>(*entity->getBehaviourByID("EquippableBehaviour"));
        if (b.isEquippableInSlot(slot)) {
            // Make sure it is in the player inventory (and in turn the entity manager)
            if (!isInInventory(entity->ID))
                Entity::addToInventory(entity);

            entity->isEquipped = true;
            equipment[slot] = entity->ID;
            return true;
        }
    }
    return false;
}

bool Entity::equip(EquipmentSlot slot, std::string ID) {
    return equip(slot, EntityManager::getInstance().getEntityByID(ID));
}

bool Entity::unequip(std::shared_ptr<Entity> entity) {
    return unequip(entity->ID);
}

bool Entity::unequip(std::string ID) {
    auto a = std::find_if(equipment.cbegin(), equipment.cend(), [ID] (auto b) { return b.second == ID; });

    if (a == equipment.cend())
        return false;
    else {
        EntityManager::getInstance().getEntityByID(a->second)->isEquipped = false;
        equipment[a->first] = "";
        return true;
    }
}

bool Entity::unequip(EquipmentSlot slot) {
    if (equipment[slot].empty())
        return false;

    EntityManager::getInstance().getEntityByID(getEquipmentID(slot))->isEquipped = false;
    equipment[slot] = "";
    return true;
}

std::shared_ptr<Entity> Entity::getEquipmentEntity(EquipmentSlot slot) const {
    if (equipment.find(slot) == equipment.cend())
        return nullptr;
    return EntityManager::getInstance().getEntityByID(equipment.at(slot));
}

std::vector<std::string> Entity::getInventoryItemsEquippableInSlot(EquipmentSlot slot) const {
    std::vector<std::string> IDs;

    std::copy_if(inventory.cbegin(), inventory.cend(), std::back_inserter(IDs), [slot] (auto &ID)
    {
        auto e = EntityManager::getInstance().getEntityByID(ID);
        if (!e->isEquipped && e->hasBehaviour("EquippableBehaviour")) {
            auto &b = dynamic_cast<EquippableBehaviour&>(*e->getBehaviourByID("EquippableBehaviour"));
            return b.isEquippableInSlot(slot);
        }
        return false;
    });

    return IDs;
}

std::string Entity::getEquipmentID(EquipmentSlot slot) const {
    return equipment.at(slot);
}

bool Entity::hasEquippedInSlot(EquipmentSlot slot) const {
    return !equipment.at(slot).empty();
}

bool Entity::hasEquipped(std::string ID) const {
    return std::find_if(equipment.cbegin(), equipment.cend(), [ID] (auto &a) { return a.second == ID; }) != equipment.cend();
}

EquipmentSlot Entity::getEquipmentSlotByID(std::string ID) const {
    auto a = std::find_if(equipment.cbegin(), equipment.cend(), [ID] (auto &a) { return a.second == ID; });

    if (a == equipment.cend())
        throw std::out_of_range("Nothing equipped with ID: " + ID);

    return a->first;
}

std::string Entity::getInventoryItemID(int inventoryIndex) const {
    return inventory[inventoryIndex];
}

int Entity::getMaxCarryWeight() const {
    auto a = getEquipmentEntity(EquipmentSlot::BACK);

    if (a != nullptr) {
        auto b = a->getBehaviourByID("AdditionalCarryWeightBehaviour");
        if (b != nullptr) {
            return maxCarryWeight + dynamic_cast<AdditionalCarryWeightBehaviour&>(*b).additionalCarryWeight;
        }
    }

    return maxCarryWeight;
}

inline std::vector<std::string> Entity::filterInventoryForCraftingMaterial(std::string materialType) const {
    return filterInventoryForCraftingMaterials(std::vector<std::string> {std::move(materialType)});
}

std::vector<std::string> Entity::filterInventoryForCraftingMaterials(const std::vector<std::string> &materialTypes) const {
    std::vector<std::string> materialIDs;

    std::copy_if(inventory.cbegin(), inventory.cend(), std::back_inserter(materialIDs),
    [materialTypes] (const auto &ID) {
        auto entity = EntityManager::getInstance().getEntityByID(ID);
        auto b = entity->getBehaviourByID("CraftingMaterialBehaviour");
        if (b != nullptr) {
            if (std::find(materialTypes.cbegin(), materialTypes.cend(),
                    dynamic_cast<CraftingMaterialBehaviour&>(*b).type) != materialTypes.cend()) {
                return true;
            }
        }
        return false;
    });

    return materialIDs;
}

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
    if (getEntityByID(entity->ID) != nullptr)
        throw std::invalid_argument("Entity with ID " + entity->ID + " already present!");

    entities[entity->ID] = entity;

    // Make sure new entities trigger a refresh of the render order list and entity caches
    if (!toRender.empty())
        recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::broadcast(Uint32 signal) {
    for (const auto& entity : entities) {
        entity.second->emit(signal);
    }
}

#define UNMANAGED_ENTITIES_ERROR_MESSAGE "Warning! The number of initialised entities is not equal to the number of entities in the entity manager!"

void EntityManager::initialize() {
    if (gNumInitialisedEntities != entities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::tick() {
    cleanup();

    timeOfDay += timePerTick;

    if (gNumInitialisedEntities != entities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    // Only update entities in this screen and surrounding screens
    // TODO: add special type of entity that always keeps updated e.g. the player's farm
    for (const auto& ID : currentlyOnScreen)
        getEntityByID(ID)->tick();
    for (const auto& ID : inSurroundingScreens)
        getEntityByID(ID)->tick();
}

void EntityManager::render(Font &font, Point currentWorldPos, LightMapTexture &lightMapTexture) {
    for (const auto& a : toRender) {
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

std::shared_ptr<Entity> EntityManager::getEntityByID(const std::string &ID) const {
    try {
        return entities.at(ID);
    } catch (const std::out_of_range &e) {
        return nullptr;
    }
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesAtPos(const Point &pos) const {
    std::vector<std::shared_ptr<Entity>> entitiesAtPos;

    for (const auto& entity : entities) {
        if (entity.second->getPos() == pos)
            entitiesAtPos.push_back(entity.second);
    }

    return entitiesAtPos;
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesAtPosFaster(const Point &pos) const {
    std::vector<std::shared_ptr<Entity>> entitiesAtPos;

    // TODO: remove ugly double loop
    for (const auto& ID : inSurroundingScreens) {
        if (getEntityByID(ID)->getPos() == pos)
            entitiesAtPos.push_back(getEntityByID(ID));
    }
    for (const auto& ID : currentlyOnScreen) {
        if (getEntityByID(ID)->getPos() == pos)
            entitiesAtPos.push_back(getEntityByID(ID));
    }

    return entitiesAtPos;
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesSurrounding(const Point &pos) const {
    std::vector<std::shared_ptr<Entity>> entitiesSurrounding;
    const std::vector<Point> surroundingPoints { pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
                                                 pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

    for (const auto& entity : entities) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), entity.second->getPos()) != surroundingPoints.cend())
            entitiesSurrounding.emplace_back(entity.second);
    }

    return entitiesSurrounding;
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesSurroundingFaster(const Point &pos) const {
    std::vector<std::shared_ptr<Entity>> entitiesSurrounding;
    const std::vector<Point> surroundingPoints { pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
                                                 pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

    // TODO: remove ugly double loop
    for (const auto& ID : inSurroundingScreens) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), getEntityByID(ID)->getPos()) != surroundingPoints.cend())
            entitiesSurrounding.emplace_back(getEntityByID(ID));
    }
    for (const auto& ID : currentlyOnScreen) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), getEntityByID(ID)->getPos()) != surroundingPoints.cend())
            entitiesSurrounding.emplace_back(getEntityByID(ID));
    }

    return entitiesSurrounding;
}

void EntityManager::cleanup() {
    while (!toBeDeleted.empty()) {
        eraseByID(toBeDeleted.front());
        toBeDeleted.pop();
    }
}

void EntityManager::queueForDeletion(const std::string &ID) {
    toBeDeleted.push(ID);
}

void EntityManager::eraseByID(const std::string &ID) {
    entities.erase(ID);
    --gNumInitialisedEntities;
    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::reorderEntities() {
    toRender.clear();
    std::transform(currentlyOnScreen.cbegin(), currentlyOnScreen.cend(), std::back_inserter(toRender),
            [this] (auto &a) -> auto { return std::make_pair(a, getEntityByID(a)->renderingLayer); });
    std::sort(toRender.begin(), toRender.end(), [](auto &a, auto &b) { return a.second > b.second; });
}

bool EntityManager::isEntityInManager(const std::string &ID) {
    return entities.find(ID) != entities.end();
}

// TODO should split this into two separate functions for current entities on screen and for surrounding screens
// player should call both current screen and surrounding screens, but other entity should only call current screen
void EntityManager::recomputeCurrentEntitiesOnScreenAndSurroundingScreens(Point currentWorldPos) {
    currentlyOnScreen.clear();
    inSurroundingScreens.clear();
    for (const auto &a : entities) {
        auto worldPosDiff = a.second->getWorldPos() - currentWorldPos;
        if (worldPosDiff == Point(0, 0))
            currentlyOnScreen.emplace_back(a.second->ID);
        else if (std::abs(worldPosDiff.x) <= 1 && std::abs(worldPosDiff.y) <= 1)
            inSurroundingScreens.emplace_back(a.second->ID);
    }
    reorderEntities();
}

const Time &EntityManager::getTimeOfDay() const {
    return timeOfDay;
}

void EntityManager::setTimeOfDay(const Time &timeOfDay) {
    EntityManager::timeOfDay = timeOfDay;
}

const Time &EntityManager::getTimePerTick() const {
    return timePerTick;
}

void EntityManager::setTimePerTick(const Time &timePerTick) {
    EntityManager::timePerTick = timePerTick;
}

std::vector<LightMapPoint> EntityManager::getLightSources(Point fontSize) const {
    std::vector<LightMapPoint> points;

    for (const auto &a : currentlyOnScreen) {
        const auto &entity = getEntityByID(a);
        if (entity->hasBehaviour("LightEmittingBehaviour")) {
            const auto &b = dynamic_cast<LightEmittingBehaviour&>(*entity->getBehaviourByID("LightEmittingBehaviour"));
            if (b.isEnabled()) {
                auto radius = fontSize.y * b.getRadius();
                auto point = fontSize * worldToScreen(entity->getPos()) + fontSize / 2;
                points.emplace_back(LightMapPoint(point, radius, b.getColor()));
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
