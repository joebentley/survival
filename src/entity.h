
#ifndef ENTITY_H_
#define ENTITY_H_

#include <vector>
#include <queue>
#include <string>
#include <cstdio>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include "font.h"
#include "world.h"
#include "point.h"
#include "flags.h"

extern int gNumInitialisedEntities;

struct Entity;

struct Behaviour {
    Behaviour(std::string ID, Entity& parent) : ID(std::move(ID)), parent(parent) {}

    bool enabled {true};
    std::string ID;
    Entity& parent;

    virtual void tick() {};
    virtual void handle(Uint32 signal) {};
};

//class ExampleBehaviour : public Behaviour {
//public:
//    ExampleBehaviour(std::string ID, Entity& parent) : Behaviour(std::move(ID), parent) {}
//
//    void initialize() {
//        printf("Initialized\n");
//    }
//
//    void tick() {
//        printf("Ticked\n");
//    }
//
//    void handle(const std::string& event) {
//        if (event == "smell")
//            printf("I can smell something\n");
//        else
//            printf("I can't smell anything\n");
//    }
//};

class EntityManager;

struct Entity {
    Entity(std::string ID, std::string name, std::string graphic,
           float hp, float maxhp, float regenPerTick, int hitTimes, int hitAmount, int maxCarryWeight)
            : hp(hp), maxhp(maxhp), regenPerTick(regenPerTick), hitTimes(hitTimes), hitAmount(hitAmount), maxCarryWeight(maxCarryWeight), ID(std::move(ID)),
              name(std::move(name)), graphic(std::move(graphic)), pos(0, 0)
    {
        if (this->ID.empty()) // use next available ID
            this->ID = std::to_string(gNumInitialisedEntities);
        gNumInitialisedEntities++;
    }

    Entity(std::string ID, std::string name, std::string graphic, float hp, float maxhp, float regenPerTick)
            : Entity(std::move(ID), std::move(name), std::move(graphic), hp, maxhp, regenPerTick, 1, 2, 100) {}

    Entity(std::string ID, std::string name, std::string graphic)
            : Entity(std::move(ID), std::move(name), std::move(graphic), 1, 1, 0, 1, 2, 100) {}

    float hp;
    float maxhp;
    float regenPerTick;
    int hitTimes;
    int hitAmount;
    bool shouldRender {true};
    int maxCarryWeight;
    bool isInAnInventory {false};
    bool isSolid {false}; // If true, cannot be walked on

    bool skipLootingDialog {false}; // automatically pick up first item in inventory when looting

    std::string ID; // Must be unique!

    float quality {1}; // Quality as a product

    std::string name;
    std::string shortDesc; // TODO: Getter for this, change based on quality?
    std::string longDesc;

    std::string graphic;
    Point pos;
    std::unordered_map<std::string, std::shared_ptr<Behaviour>> behaviours;

    int renderingLayer {0};

    virtual void addBehaviour(std::shared_ptr<Behaviour> behaviour);
    virtual void tick();
    virtual void destroy();
    virtual void emit(uint32_t signal);
    virtual void render(Font& font, Point currentWorldPos);

    virtual bool addToInventory(const std::shared_ptr<Entity> &item);
    void removeFromInventory(const std::string &ID);
    void removeFromInventory(int inventoryIndex);
    void dropItem(int inventoryIndex);
    std::shared_ptr<Entity> getInventoryItem(int inventoryIndex) const;
    size_t getInventorySize() const;
    bool isInventoryEmpty() const;
    std::vector<std::shared_ptr<Entity>> getInventory() const;
    bool isInInventory(std::string ID) const;

    void setPos(int x, int y) { pos = Point(x, y); }
    void setPos(Point p) { pos = p; }

    inline Point getWorldPos() {
        return { this->pos.x / SCREEN_WIDTH, this->pos.y / SCREEN_HEIGHT };
    }

    std::shared_ptr<Behaviour> getBehaviourByID(const std::string& ID) const;
    bool hasBehaviour(const std::string& ID) const;

    int rollDamage();

    int getCarryingWeight();

    void addHealth(float health);

protected:
    std::vector<std::string> inventory;
};

// Singleton class that manages all entities
class EntityManager {
    std::vector<std::pair<std::string, std::shared_ptr<Entity>>> toRender;

    void reorderEntities();
public:
    static EntityManager& getInstance() {
        static EntityManager instance;
        return instance;
    }

    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    void operator=(const EntityManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Entity>> entities;
    std::queue<std::string> toBeDeleted;

    void addEntity(std::shared_ptr<Entity> entity);
    void broadcast(uint32_t signal);
    void initialize();
    void tick();
    void cleanup(); // Cleanup entities to be deleted
    virtual void destroy() { }
    void render(Font& font, Point currentWorldPos);
    void render(Font& font);
    std::shared_ptr<Entity> getEntityByID(const std::string &ID) const;
    void queueForDeletion(const std::string &ID);
    void eraseByID(const std::string &ID);
    std::vector<std::shared_ptr<Entity>> getEntitiesAtPos(const Point& pos) const;
};

#endif // ENTITY_H_