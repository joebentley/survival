
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
#include "time.h"
#include "texture.h"

extern int gNumInitialisedEntities;

struct Entity;

struct Behaviour {
    Behaviour(std::string ID, Entity& parent) : ID(std::move(ID)), parent(parent) {}

    std::string ID;
    Entity& parent;

    virtual void tick() {};
    virtual void handle(Uint32 signal) {};

    virtual bool isEnabled() const {
        return enabled;
    }

    void enable() {
        enabled = true;
    }

    void disable() {
        enabled = false;
    }
protected:
    bool enabled {true};
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

enum class EquipmentSlot {
    HEAD,
    TORSO,
    LEGS,
    RIGHT_HAND,
    LEFT_HAND,
    FEET,
    BACK
};

EquipmentSlot &operator++(EquipmentSlot &slot);

EquipmentSlot &operator--(EquipmentSlot &slot);

const std::vector<EquipmentSlot> EQUIPMENT_SLOTS {
    EquipmentSlot::HEAD, EquipmentSlot::TORSO, EquipmentSlot::LEGS,
    EquipmentSlot::RIGHT_HAND, EquipmentSlot::LEFT_HAND, EquipmentSlot::FEET,
    EquipmentSlot::BACK
};

inline std::string slotToString(EquipmentSlot slot) {
    if (slot == EquipmentSlot::HEAD)
        return "Head";
    if (slot == EquipmentSlot::TORSO)
        return "Torso";
    if (slot == EquipmentSlot::LEGS)
        return "Legs";
    if (slot == EquipmentSlot::RIGHT_HAND)
        return "Right hand";
    if (slot == EquipmentSlot::LEFT_HAND)
        return "Left hand";
    if (slot == EquipmentSlot::FEET)
        return "Feet";
    if (slot == EquipmentSlot::BACK)
        return "Back";
    return "";
}

class EntityManager;
struct Entity {
    Entity(std::string ID, std::string name, std::string graphic,
           float hp, float maxhp, float regenPerTick, int hitTimes, int hitAmount, int maxCarryWeight)
            : hp(hp), maxhp(maxhp), regenPerTick(regenPerTick), hitTimes(hitTimes), hitAmount(hitAmount), ID(std::move(ID)),
              name(std::move(name)), graphic(std::move(graphic)), pos(0, 0), maxCarryWeight(maxCarryWeight)
    {
        if (this->ID.empty())
            this->ID = std::to_string(rand());
        gNumInitialisedEntities++;

        // Add all equipment slots
        equipment[EquipmentSlot::HEAD] = "";
        equipment[EquipmentSlot::TORSO] = "";
        equipment[EquipmentSlot::LEGS] = "";
        equipment[EquipmentSlot::RIGHT_HAND] = "";
        equipment[EquipmentSlot::LEFT_HAND] = "";
        equipment[EquipmentSlot::FEET] = "";
        equipment[EquipmentSlot::BACK] = "";
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
    bool isInAnInventory {false};
    bool isEquipped {false};
    bool isSolid {false}; // If true, cannot be walked on

    bool skipLootingDialog {false}; // automatically pick up first item in inventory when looting

    std::string ID; // Must be unique!

    float quality {1}; // Quality as a product

    std::string name;
    std::string shortDesc; // TODO: Getter for this, change based on quality?
    std::string longDesc;

    std::string graphic;
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
    std::string getInventoryItemID(int inventoryIndex) const;
    size_t getInventorySize() const;
    bool isInventoryEmpty() const;
    std::vector<std::shared_ptr<Entity>> getInventory() const;
    bool isInInventory(std::string ID) const;

    /// Filter inventory for crafting material of type `materialType` returning a vector of ID strings
    /// \param materialType type of material to filter for
    /// \return list of IDs of those materials
    std::vector<std::string> filterInventoryForCraftingMaterial(std::string materialType) const;

    /// Same as `filterInventoryForCraftingMaterial` but match any of the types in `materialTypes`
    /// \param materialTypes types of material to filter for
    /// \return list of IDs of those materials
    std::vector<std::string> filterInventoryForCraftingMaterials(const std::vector<std::string> &materialTypes) const;

    int getCarryingWeight();

    void setPos(int x, int y) { pos = Point(x, y); }
    void setPos(Point p) { pos = p; }
    Point getPos() const;
    Point getWorldPos() const {
        return { this->pos.x / SCREEN_WIDTH, this->pos.y / SCREEN_HEIGHT };
    }

    int getMaxCarryWeight() const;

    /// Attempt to move to Point P. Will not move to the point if there is a solid entity in the way
    /// \param p Point to move to
    /// \return Whether or not the movement was performed
    bool moveTo(Point p);

    std::shared_ptr<Behaviour> getBehaviourByID(const std::string& ID) const;
    bool hasBehaviour(const std::string& ID) const;

    int computeMaxDamage() const;
    int rollDamage();

    void addHealth(float health);

    const std::unordered_map<EquipmentSlot, std::string> &getEquipment() const;
    std::string getEquipmentID(EquipmentSlot slot) const;
    /// Return shared pointer to entity in slot, returning nullptr if no entity in slot
    /// \param slot EquipmentSlot to look in
    /// \return shared_ptr to entity in slot
    std::shared_ptr<Entity> getEquipmentEntity(EquipmentSlot slot) const;
    bool equip(EquipmentSlot slot, std::shared_ptr<Entity> entity);
    bool equip(EquipmentSlot slot, std::string ID);
    bool unequip(std::shared_ptr<Entity> entity);
    bool unequip(std::string ID);
    bool unequip(EquipmentSlot slot);
    std::vector<std::string> getInventoryItemsEquippableInSlot(EquipmentSlot slot) const;
    bool hasEquippedInSlot(EquipmentSlot slot) const;
    bool hasEquipped(std::string ID) const;
    EquipmentSlot getEquipmentSlotByID(std::string ID) const;

protected:
    std::vector<std::string> inventory;
    Point pos;
    std::unordered_map<EquipmentSlot, std::string> equipment;
    int maxCarryWeight;
};

/// Singleton class that manages all entities
class EntityManager {
    std::unordered_map<std::string, std::shared_ptr<Entity>> entities;
    std::queue<std::string> toBeDeleted;
    std::vector<std::string> currentlyOnScreen;
    std::vector<std::string> inSurroundingScreens;
    std::vector<std::pair<std::string, int>> toRender;

    Time timeOfDay;
    Time timePerTick { 0, 2 };

    void reorderEntities();
public:
    static EntityManager& getInstance() {
        static EntityManager instance;
        return instance;
    }

    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    void operator=(const EntityManager&) = delete;

    void addEntity(std::shared_ptr<Entity> entity);
    void broadcast(uint32_t signal);
    void initialize();
    void tick();
    void cleanup(); // Cleanup entities to be deleted

    void render(Font &font, Point currentWorldPos, LightMapTexture &lightMapTexture);
    void render(Font &font, LightMapTexture &lightMapTexture);

    std::shared_ptr<Entity> getEntityByID(const std::string &ID) const;
    void queueForDeletion(const std::string &ID);
    void eraseByID(const std::string &ID);

    /// Find entities at point `pos` WARNING SLOW (reads every entity)
    /// \param pos position to look at
    /// \return vector of shared pointers at pos
    std::vector<std::shared_ptr<Entity>> getEntitiesAtPos(const Point& pos) const;

    /// Find entities at point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look at
    /// \return vector of shared pointers at pos
    std::vector<std::shared_ptr<Entity>> getEntitiesAtPosFaster(const Point& pos) const;

    /// Find entities surrounding point `pos` WARNING SLOW (reads every entity)
    /// \param pos position to look around
    /// \return vector of shared pointers to entities surrounding pos
    std::vector<std::shared_ptr<Entity>> getEntitiesSurrounding(const Point& pos) const;

    /// Find entities surrounding point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look around
    /// \return vector of shared pointers to entities surrounding pos
    std::vector<std::shared_ptr<Entity>> getEntitiesSurroundingFaster(const Point& pos) const;

    bool isEntityInManager(const std::string &ID);

    /// Should be called every time the player changes screen. Recomputes current entities on this screen and surrounding screens
    /// \param currentWorldPos current position in world space
    void recomputeCurrentEntitiesOnScreenAndSurroundingScreens(Point currentWorldPos);

    /// Same as `recomputeCurrentEntitiesOnScreenAndSurroundingScreens` but uses player's currentWorldPos
    void recomputeCurrentEntitiesOnScreenAndSurroundingScreens();

    /// Get current time of day
    /// \return time of day
    const Time &getTimeOfDay() const;
    /// Set time of day
    /// \param timeOfDay current time of day
    void setTimeOfDay(const Time &timeOfDay);
    /// Get time increment per EntityManager tick
    /// \return increment per tick
    const Time &getTimePerTick() const;
    /// Set time increment per EntityManager tick
    /// \param timePerTick increment per tick
    void setTimePerTick(const Time &timePerTick);

    /// Get light sources on screen in screen-space coords.
    /// Assumes that recomputeCurrentEntitiesOnScreen has been called to generate the vector of entities on screen
    /// \param fontSize Point representing the width and height of each font cell
    /// \return vector of light sources on screen
    std::vector<LightMapPoint> getLightSources(Point fontSize) const;
};

#endif // ENTITY_H_