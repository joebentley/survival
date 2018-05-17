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
        if (behaviour.second->enabled)
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

int Entity::rollDamage() {
    int totalDamage = 0;
    for (int i = 0; i < hitTimes; ++i) {
        totalDamage += rand() % (hitAmount + 1);
    }
    return totalDamage;
}

bool Entity::addToInventory(const std::shared_ptr<Entity> &item) {
    auto b = item->getBehaviourByID("PickuppableBehaviour");
    if (b != nullptr) {
        if (!EntityManager::getInstance().isEntityInManager(item->ID))
            EntityManager::getInstance().addEntity(item);

        if (getCarryingWeight() + dynamic_cast<PickuppableBehaviour&>(*b).weight > maxCarryWeight)
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
    return std::find(inventory.cbegin(), inventory.cend(), ID) == inventory.cend();
}

Point Entity::getPos() const {
    return pos;
}

bool Entity::moveTo(Point p) {
    auto entities = EntityManager::getInstance().getEntitiesAtPos(p);

    // Check if there is a solid object in space
    if (std::find_if(entities.cbegin(), entities.cend(), [] (auto &a) { return a->isSolid; }) == entities.cend()) {
        setPos(p);
        return true;
    }
    return false;
}

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
    if (getEntityByID(entity->ID) != nullptr)
        throw std::invalid_argument("Entity with ID " + entity->ID + " already present!");

    entities[entity->ID] = entity;

    // Make sure new entities trigger a refresh of the render order list
    if (!toRender.empty())
        reorderEntities();
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

    recomputeCurrentEntitiesOnScreen(getEntityByID("Player")->getWorldPos());
    reorderEntities(); // Order the entities by rendering layer
}

void EntityManager::tick() {
    cleanup();

    timeOfDay += timePerTick;

    if (gNumInitialisedEntities != entities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    for (const auto& entity : entities) {
        entity.second->tick();
    }
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
    lightMapTexture.render(EntityManager::getInstance().getLightSources(), alpha);
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
    recomputeCurrentEntitiesOnScreen(getEntityByID("Player")->getWorldPos());
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

void EntityManager::recomputeCurrentEntitiesOnScreen(Point currentWorldPos) {
    currentlyOnScreen.clear();
    for (const auto &a : entities) {
        if (a.second->getWorldPos() == currentWorldPos)
            currentlyOnScreen.emplace_back(a.second->ID);
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

std::vector<LightMapPoint> EntityManager::getLightSources() const {
    std::vector<LightMapPoint> points;

    for (const auto &a : currentlyOnScreen) {
        const auto &entity = getEntityByID(a);
        if (entity->hasBehaviour("LightEmittingBehaviour")) {
            const auto &b = dynamic_cast<LightEmittingBehaviour&>(*entity->getBehaviourByID("LightEmittingBehaviour"));
            auto radius = b.getRadius();
            auto point = worldToScreen(entity->getPos());
            points.emplace_back(LightMapPoint(point, radius));
        }
    }

    return points;
}
