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

void Entity::render(Font& font) {
    font.drawText(graphic, x, y);
}

void EntityManager::addEntity(Entity& entity) {
    entities.push_back(entity);
}

void EntityManager::broadcast(const std::string& event) {
    for (auto entity : entities) {
        entity.emit(event);
    }
}

void EntityManager::initialize() {
    for (auto entity : entities) {
        entity.initialize();
    }
}

void EntityManager::tick() {
    for (auto entity : entities) {
        entity.tick();
    }
}
