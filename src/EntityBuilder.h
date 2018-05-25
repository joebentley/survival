#ifndef SURVIVAL_ENTITYBUILDER_H
#define SURVIVAL_ENTITYBUILDER_H

#include "entity.h"

namespace EntityBuilder {
    template<typename T>
    void makeEntityAndAddToInventory(Entity *entityToAddTo) {
        auto item = std::make_unique<T>();
        auto ID = item->mID;
        EntityManager::getInstance().addEntity(std::move(item));
        entityToAddTo->Entity::addToInventory(ID);
    }

    template<typename T>
    T *makeEntity() {
        auto entity = std::make_unique<T>();
        auto p = entity.get();
        EntityManager::getInstance().addEntity(std::move(entity));
        return p;
    }

    Entity *makeEntity(std::string ID, std::string name, std::string graphic) {
        auto entity = std::make_unique<Entity>(ID, name, graphic);
        auto p = entity.get();
        EntityManager::getInstance().addEntity(std::move(entity));
        return p;
    }
}

#endif //SURVIVAL_ENTITYBUILDER_H
