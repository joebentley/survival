
#ifndef ENTITY_H_
#define ENTITY_H_

#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <memory>
#include "font.h"

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
        : ID(ID), graphic(graphic), x(0), y(0), worldX(0), worldY(0), entityManager(entityManager) {}

    std::string ID;
    std::string graphic;
    int x;
    int y;
    int worldX;
    int worldY;
    EntityManager& entityManager;
    std::vector<Behaviour*> behaviours;

    void addBehaviour(Behaviour* behaviour);
    void initialize();
    void tick();
    void destroy();
    void emit(const std::string& event);
    void render(Font& font, int currentWorldX, int currentWorldY);
    void setPos(int x, int y) { this->x = x; this->y = y; }
    void setWorldPos(int worldX, int worldY) { this->worldX = worldX; this->worldY = worldY; }    
};

class EntityManager {
public:
    std::vector<Entity*> entities;
    void addEntity(Entity* entity);
    void broadcast(const std::string& event);
    void initialize();
    void tick();
    void destroy();
    void render(Font& font, int currentWorldX, int currentWorldY);
};

#endif // ENTITY_H_