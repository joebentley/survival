
#ifndef ENTITY_H_
#define ENTITY_H_

#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <memory>
#include "font.h"
#include "world.h"

class Entity;

class Behaviour {
public:
    Behaviour(const std::string& ID, Entity& parent) : ID(ID), parent(parent) {}

    std::string ID;
    Entity& parent;
    virtual void initialize() {};
    virtual void tick() {};
    virtual void handle(const std::string& event) {};
};

class ExampleBehaviour : public Behaviour {
public:
    ExampleBehaviour(const std::string& ID, Entity& parent) : Behaviour(ID, parent) {}

    void initialize() {
        printf("Initialized\n");
    }

    void tick() {
        printf("Ticked\n");
    }

    void handle(const std::string& event) {
        if (event == "smell")
            printf("I can smell something\n");
        else
            printf("I can't smell anything\n");
    }
};

class EntityManager;

class Entity {
public:
    Entity(const std::string& ID, const std::string& graphic, EntityManager& entityManager)
        : ID(ID), graphic(graphic), x(0), y(0), entityManager(entityManager) {}

    std::string ID;
    std::string graphic;
    int x;
    int y;
    EntityManager& entityManager;
    std::vector<std::unique_ptr<Behaviour>> behaviours;

    void addBehaviour(std::unique_ptr<Behaviour>& behaviour);
    void initialize();
    void tick();
    void destroy();
    void emit(const std::string& event);
    void render(Font& font, int currentWorldX, int currentWorldY);
    void render(Font& font, std::tuple<int, int> currentWorldPos) {
        render(font, std::get<0>(currentWorldPos), std::get<1>(currentWorldPos));
    }
    void setPos(int x, int y) { this->x = x; this->y = y; }

    std::tuple<int, int> getWorldPos() {
        return std::make_tuple(this->x / SCREEN_WIDTH, this->y / SCREEN_HEIGHT);
    }
};

class EntityManager {
public:
    std::vector<std::shared_ptr<Entity>> entities;
    void addEntity(std::shared_ptr<Entity> entity);
    void broadcast(const std::string& event);
    void initialize();
    void tick();
    void destroy();
    void render(Font& font, int currentWorldX, int currentWorldY);
    void render(Font& font, std::tuple<int, int> currentWorldPos) {
        render(font, std::get<0>(currentWorldPos), std::get<1>(currentWorldPos));
    }
};

#endif // ENTITY_H_