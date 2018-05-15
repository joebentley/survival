#include "entity.h"
#include "behaviours.h"

#include <stdexcept>

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

bool Entity::addToInventory(std::shared_ptr<Entity> item) {
    auto b = item->getBehaviourByID("PickuppableBehaviour");
    if (b != nullptr) {
        if (EntityManager::getInstance().entities.find(item->ID) == EntityManager::getInstance().entities.end())
            EntityManager::getInstance().addEntity(item);

        if (getCarryingWeight() + dynamic_cast<PickuppableBehaviour&>(*b).weight > maxCarryWeight)
            return false;
        item->setPos(pos);
        inventory.push_back(item);
        item->shouldRender = false;
        item->isInAnInventory = true;
        return true;
    } else {
        throw std::invalid_argument("item does not have PickuppableBehaviour");
    }
}

void Entity::dropItem(int inventoryIndex) {
    auto item = inventory[inventoryIndex];

    inventory.erase(inventory.begin() + inventoryIndex);
    item->shouldRender = true;
    item->isInAnInventory = false;
    item->setPos(pos);
}

bool Entity::hasBehaviour(const std::string &ID) const {
    return behaviours.find(ID) != behaviours.end();
}

void Entity::destroy() {
    EntityManager::getInstance().eraseByID(ID);
}

int Entity::getCarryingWeight() {
    int totalWeight = 0;
    for (const auto& item : inventory) {
        if (item->hasBehaviour("PickuppableBehaviour")) {
            auto a = dynamic_cast<PickuppableBehaviour&>(*item->getBehaviourByID("PickuppableBehaviour"));
            totalWeight += a.weight;
        }
    }
    return totalWeight;
}

void Entity::removeFromInventoryByID(const std::string &ID) {
    inventory.erase(std::remove_if(inventory.begin(), inventory.end(), [ID] (auto &a) { return a->ID == ID; }),
                    inventory.end());
}

void Entity::addHealth(float health) {
    this->hp = std::min(this->hp + health, maxhp);
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

    reorderEntities(); // Order the entities by rendering layer
}

void EntityManager::tick() {
    cleanup();

    if (gNumInitialisedEntities != entities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    for (const auto& entity : entities) {
        entity.second->tick();
    }
}

void EntityManager::render(Font& font, Point currentWorldPos) {
    for (const auto& entity : toRender) {
        entity.second->render(font, currentWorldPos);
    }
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
        if (entity.second->pos == pos)
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
    reorderEntities(); // Order the entities by rendering layer
}

void EntityManager::reorderEntities() {
    toRender = std::vector<std::pair<std::string, std::shared_ptr<Entity>>>(entities.begin(), entities.end());
    std::sort(toRender.begin(), toRender.end(), [](auto &a, auto &b) { return a.second->renderingLayer > b.second->renderingLayer; });
}
