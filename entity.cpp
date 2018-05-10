#include "entity.h"

void Entity::addBehaviour(Behaviour* behaviour) {
    behaviours.push_back(behaviour);
}

void Entity::initialize() {
    for (auto behaviour : behaviours) {
        behaviour->initialize();
    }
}

void Entity::tick() {
    for (auto behaviour : behaviours) {
        behaviour->tick();
    }
}

void Entity::emit(const std::string& event) {
    for (auto behaviour : behaviours) {
        behaviour->handle(event);
    }
}

void Entity::render(Font& font, int currentWorldX, int currentWorldY) {
    // Only draw if the entity is on the current world screen
    if (worldX == currentWorldX && worldY == currentWorldY)
        font.drawText(graphic, this->x, this->y);
}

void EntityManager::addEntity(Entity* entity) {
    entities.push_back(entity);
}

void EntityManager::broadcast(const std::string& event) {
    for (auto entity : entities) {
        entity->emit(event);
    }
}

void EntityManager::initialize() {
    for (auto entity : entities) {
        entity->initialize();
    }
}

void EntityManager::tick() {
    for (auto entity : entities) {
        entity->tick();
    }
}

void EntityManager::render(Font& font, int currentWorldX, int currentWorldY) {
    for (auto entity : entities) {
        entity->render(font, currentWorldX, currentWorldY);
    }
}
