
#ifndef ENTITY_H_
#define ENTITY_H_

#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <memory>
#include "font.h"
#include "world.h"
#include "point.h"
#include "flags.h"

class Entity;

class Behaviour {
public:
    Behaviour(std::string ID, Entity& parent) : ID(std::move(ID)), parent(parent) {}

    std::string ID;
    Entity& parent;
    virtual void initialize() {};
    virtual void tick() {};
    virtual void handle(uint32_t signal) {};
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

class Entity {
public:
    Entity(std::string ID, std::string graphic, EntityManager& entityManager, int hp, int maxhp)
            : hp(hp), maxhp(maxhp), ID(std::move(ID)), graphic(std::move(graphic)), pos(0, 0), manager(entityManager) {}

    Entity(std::string ID, std::string graphic, EntityManager& entityManager)
            : Entity(ID, graphic, entityManager, 0, 0) {}

    int hp;
    int maxhp;
    std::string ID;
    std::string graphic;
    Point pos;
    EntityManager& manager;
    std::vector<std::unique_ptr<Behaviour>> behaviours;

    virtual void addBehaviour(std::unique_ptr<Behaviour>& behaviour);
    virtual void initialize();
    virtual void tick();
    virtual void destroy() { }
    virtual void emit(uint32_t signal);
    virtual void render(Font& font, int currentWorldX, int currentWorldY);
    virtual void render(Font& font, std::tuple<int, int> currentWorldPos) {
        render(font, std::get<0>(currentWorldPos), std::get<1>(currentWorldPos));
    }
    void setPos(int x, int y) { pos = Point(x, y); }

    std::tuple<int, int> getWorldPos() {
        return std::make_tuple(this->pos.x / SCREEN_WIDTH, this->pos.y / SCREEN_HEIGHT);
    }
};

class EntityManager {
public:
    std::vector<std::shared_ptr<Entity>> entities;
    void addEntity(std::shared_ptr<Entity> entity);
    void broadcast(uint32_t signal);
    void initialize();
    void tick();
    virtual void destroy() { }
    void render(Font& font, int currentWorldX, int currentWorldY);
    void render(Font& font, std::tuple<int, int> currentWorldPos) {
        render(font, std::get<0>(currentWorldPos), std::get<1>(currentWorldPos));
    }
    std::shared_ptr<Entity> getByID(const std::string& ID) const;
    std::vector<std::shared_ptr<Entity>> getEntitiesAtPos(const Point& pos) const;
};

#endif // ENTITY_H_