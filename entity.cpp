#include "entity.h"

void Entity::addBehaviour(std::unique_ptr<Behaviour>& behaviour) {
    behaviours.push_back(std::move(behaviour));
}

void Entity::initialize() {
    for (auto& behaviour : behaviours) {
        behaviour->initialize();
    }
}

void Entity::tick() {
    for (auto& behaviour : behaviours) {
        behaviour->tick();
    }
}

void Entity::emit(const std::string& event) {
    for (auto& behaviour : behaviours) {
        behaviour->handle(event);
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

void EntityManager::addEntity(std::shared_ptr<Entity> entity) {
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

std::shared_ptr<Entity> EntityManager::getByID(const std::string& ID) const {
    auto entity = std::find_if(entities.begin(), entities.end(), [ID](auto& a) { return a->ID == ID; });

    if (entity == entities.end())
        return nullptr;
    else
        return *entity;
}
