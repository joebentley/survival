
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

    virtual void initialize() {};
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
    Entity(std::string ID, std::string type, std::string name, std::string graphic,
           double hp, double maxhp, double regenPerTick, int hitTimes, int hitAmount, int maxCarryWeight)
            : hp(hp), maxhp(maxhp), regenPerTick(regenPerTick), hitTimes(hitTimes), hitAmount(hitAmount), maxCarryWeight(maxCarryWeight), ID(std::move(ID)),
              type(std::move(type)), name(std::move(name)), graphic(std::move(graphic)), pos(0, 0)
    {
        if (this->ID.empty()) // use next available ID
            this->ID = std::to_string(gNumInitialisedEntities);
        gNumInitialisedEntities++;
    }

    Entity(std::string ID, std::string type, std::string name, std::string graphic, double hp, double maxhp, double regenPerTick)
            : Entity(std::move(ID), std::move(type), std::move(name), std::move(graphic), hp, maxhp, regenPerTick, 1, 2, 100) {}

    Entity(std::string ID, std::string type, std::string name, std::string graphic)
            : Entity(std::move(ID), std::move(type), std::move(name), std::move(graphic), 1, 1, 0, 1, 2, 100) {}

    double hp;
    double maxhp;
    double regenPerTick;
    int hitTimes;
    int hitAmount;
    bool shouldRender {true};
    int maxCarryWeight;
    bool isInAnInventory {false};

    bool skipLootingDialog {false}; // automatically pick up first item in inventory when looting

    std::string ID; // Must be unique!

    // TODO: move these to a CraftingMaterialBehaviour
    std::string type; // Used in crafting recipes
    float quality {1}; // Quality as a construction material

    std::string name;
    std::string shortDesc;
    std::string longDesc;

    std::string graphic;
    Point pos;
    std::unordered_map<std::string, std::shared_ptr<Behaviour>> behaviours;
    std::vector<std::shared_ptr<Entity>> inventory;

    int renderingLayer {0};

    virtual void addBehaviour(std::shared_ptr<Behaviour> behaviour);
    virtual void initialize();
    virtual void tick();
    virtual void destroy();
    virtual void emit(uint32_t signal);
    virtual void render(Font& font, Point currentWorldPos);

    virtual bool addToInventory(std::shared_ptr<Entity> item);
    void dropItem(int inventoryIndex);

    void setPos(int x, int y) { pos = Point(x, y); }
    void setPos(Point p) { pos = p; }

    Point getWorldPos() {
        return { this->pos.x / SCREEN_WIDTH, this->pos.y / SCREEN_HEIGHT };
    }

    std::shared_ptr<Behaviour> getBehaviourByID(const std::string& ID) const;
    bool hasBehaviour(const std::string& ID) const;

    int rollDamage();

    int getCarryingWeight();
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
    std::shared_ptr<Entity> getEntityByID(const std::string &ID) const;
    void queueForDeletion(const std::string &ID);
    void eraseByID(const std::string &ID);
    std::vector<std::shared_ptr<Entity>> getEntitiesAtPos(const Point& pos) const;
};

#endif // ENTITY_H_