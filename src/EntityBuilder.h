#ifndef SURVIVAL_ENTITYBUILDER_H
#define SURVIVAL_ENTITYBUILDER_H

#include "Entity.h"

// TODO: do these need to handle raw pointers at all?
namespace EntityBuilder {
    /// Make a new instance of an item entity without passing any parameters to the constructor.
    /// The item is then added to the EntityManager and put into the inventory of `entityToAddTo`.
    /// \tparam T the item entity to initialize
    /// \param entityToAddTo the entity whose inventory the item should be added to
    template<typename T>
    void makeEntityAndAddToInventory(Entity *entityToAddTo) {
        auto item = std::make_unique<T>();
        auto ID = item->mID;
        EntityManager::getInstance().addEntity(std::move(item));
        entityToAddTo->Entity::addToInventory(ID);
    }

    /// Make a new instance of the entity without passing any parameters to the constructor, adding it to the
    /// EntityManager and returning a raw pointer reference.
    /// \tparam T the entity to initialize
    /// \return a raw unowned pointer to the initialized entity
    template<typename T>
    T *makeEntity() {
        auto entity = std::make_unique<T>();
        auto p = entity.get();
        EntityManager::getInstance().addEntity(std::move(entity));
        return p;
    }

    /// Initialize a new Entity base class with given parameters, adding it to the EntityManager.
    /// \param ID the ID to pass on to the Entity constructor
    /// \param name the name to pass on to the Entity constructor as a descriptive name
    /// \param graphic the graphic to pass on to the Entity constructor for rendering
    /// \return a raw unowned pointer to the initialized entity
    Entity *makeEntity(std::string ID, std::string name, std::string graphic) {
        auto entity = std::make_unique<Entity>(ID, name, graphic);
        auto p = entity.get();
        EntityManager::getInstance().addEntity(std::move(entity));
        return p;
    }
}

#endif //SURVIVAL_ENTITYBUILDER_H
