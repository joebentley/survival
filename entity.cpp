#include "entity.h"

void Entity::addBehaviour(std::shared_ptr<Behaviour>& behaviour) {
    behaviours.push_back(behaviour);
}

void Entity::initialize() {
    for (auto& behaviour : behaviours) {
        behaviour->initialize();
    }
}

void Entity::tick() {
    if (hp < maxhp)
        hp += regenPerTick;

    if (hp > maxhp)
        hp = maxhp;

    for (auto& behaviour : behaviours) {
        if (behaviour->enabled)
            behaviour->tick();
    }
}

void Entity::emit(Uint32 signal) {
    for (auto& behaviour : behaviours) {
        behaviour->handle(signal);
    }
}

void Entity::render(Font& font, int currentWorldX, int currentWorldY) {
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
    auto behaviour = std::find_if(behaviours.begin(), behaviours.end(), [ID](auto& a) { return a->ID == ID; });

    if (behaviour == behaviours.end())
        return nullptr;
    else
        return *behaviour;
}

int Entity::rollDamage() {
    int totalDamage = 0;
    for (int i = 0; i < hitTimes; ++i) {
        totalDamage += rand() % (hitAmount + 1);
    }
    return totalDamage;
}

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);

    // Sort by rendering layer
    std::sort(entities.begin(), entities.end(), [](auto& a, auto& b) { return a->renderingLayer > b->renderingLayer; });
}

void EntityManager::broadcast(Uint32 signal) {
    for (const auto& entity : entities) {
        entity->emit(signal);
    }
}

void EntityManager::initialize() {
    for (const auto& entity : entities) {
        entity->initialize();
    }
}

void EntityManager::tick() {
    for (const auto& entity : entities) {
        entity->tick();
    }
}

void EntityManager::render(Font& font, int currentWorldX, int currentWorldY) {
    for (const auto& entity : entities) {
        entity->render(font, currentWorldX, currentWorldY);
    }
}

std::shared_ptr<Entity> EntityManager::getByID(const std::string& ID) const {
    auto entity = std::find_if(entities.begin(), entities.end(), [ID](auto& a) { return a->ID == ID; });

    if (entity == entities.end())
        return nullptr;
    else
        return *entity;
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntitiesAtPos(const Point &pos) const {
    std::vector<std::shared_ptr<Entity>> entitiesAtPos;

    for (const auto& entity : entities) {
        if (entity->pos == pos)
            entitiesAtPos.push_back(entity);
    }

    return entitiesAtPos;
}

void EntityManager::cleanup() {
    while (!toBeDeleted.empty()) {
        eraseByID(toBeDeleted.front());
        toBeDeleted.pop();
    }
}

void EntityManager::queueForDeletion(const std::string& ID) {
    toBeDeleted.push(ID);
}

void EntityManager::eraseByID(const std::string &ID) {
    auto it = std::find_if(entities.begin(), entities.end(), [ID](auto& a) { return a->ID == ID; });

    if (it != entities.end())
        entities.erase(it);
}
