
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
#include "Font.h"
#include "world.h"
#include "Point.h"
#include "flags.h"
#include "time.h"
#include "Texture.h"

extern int gNumInitialisedEntities;

struct Entity;

struct Behaviour {
    Behaviour(std::string ID, Entity& parent) : mID(std::move(ID)), mParent(parent) {}

    std::string mID;
    Entity& mParent;

    virtual void tick() {};
    virtual void handle(Uint32 signal) {};

    virtual bool isEnabled() const {
        return mEnabled;
    }

    void enable() {
        mEnabled = true;
    }

    void disable() {
        mEnabled = false;
    }
protected:
    bool mEnabled {true};
};

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
            : mHp(hp), mMaxHp(maxhp), mRegenPerTick(regenPerTick), mHitTimes(hitTimes), mHitAmount(hitAmount), mID(std::move(ID)),
              mName(std::move(name)), mGraphic(std::move(graphic)), mPos(0, 0), mMaxCarryWeight(maxCarryWeight)
    {
        if (this->mID.empty())
            this->mID = std::to_string(rand());
        gNumInitialisedEntities++;

        // Add all equipment slots
        mEquipment[EquipmentSlot::HEAD] = "";
        mEquipment[EquipmentSlot::TORSO] = "";
        mEquipment[EquipmentSlot::LEGS] = "";
        mEquipment[EquipmentSlot::RIGHT_HAND] = "";
        mEquipment[EquipmentSlot::LEFT_HAND] = "";
        mEquipment[EquipmentSlot::FEET] = "";
        mEquipment[EquipmentSlot::BACK] = "";
    }

    Entity(std::string ID, std::string name, std::string graphic, float hp, float maxhp, float regenPerTick)
            : Entity(std::move(ID), std::move(name), std::move(graphic), hp, maxhp, regenPerTick, 1, 2, 100) {}

    Entity(std::string ID, std::string name, std::string graphic)
            : Entity(std::move(ID), std::move(name), std::move(graphic), 1, 1, 0, 1, 2, 100) {}

    float mHp;
    float mMaxHp;
    float mRegenPerTick;
    int mHitTimes;
    int mHitAmount;
    bool mShouldRender {true};
    bool mIsInAnInventory {false};
    bool mIsEquipped {false};
    bool mIsSolid {false}; // If true, cannot be walked on

    bool mSkipLootingDialog {false}; // automatically pick up first item in inventory when looting

    std::string mID; // Must be unique!

    float mQuality {1}; // Quality as a product

    std::string mName;
    std::string mShortDesc; // TODO: Getter for this, change based on quality?
    std::string mLongDesc;

    std::string mGraphic;

    int mRenderingLayer {0};

    virtual void addBehaviour(std::unique_ptr<Behaviour> behaviour);
    virtual void tick();
    virtual void destroy();
    virtual void emit(uint32_t signal);
    virtual void render(Font& font, Point currentWorldPos);

    virtual bool addToInventory(const std::string &ID);
    void removeFromInventory(const std::string &ID);
    void removeFromInventory(int inventoryIndex);
    void dropItem(int inventoryIndex);
    Entity * getInventoryItem(int inventoryIndex) const;
    std::string getInventoryItemID(int inventoryIndex) const;
    size_t getInventorySize() const;
    bool isInventoryEmpty() const;
    std::vector<Entity *> getInventory() const;
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

    void setPos(int x, int y) { mPos = Point(x, y); }
    void setPos(Point p) { mPos = p; }
    Point getPos() const;
    Point getWorldPos() const {
        return { this->mPos.mX / SCREEN_WIDTH, this->mPos.mY / SCREEN_HEIGHT };
    }

    int getMaxCarryWeight() const;

    /// Attempt to move to Point P. Will not move to the point if there is a solid entity in the way
    /// \param p Point to move to
    /// \return Whether or not the movement was performed
    bool moveTo(Point p);

    Behaviour * getBehaviourByID(const std::string &ID) const;
    bool hasBehaviour(const std::string& ID) const;

    int computeMaxDamage() const;
    int rollDamage();

    void addHealth(float health);

    const std::unordered_map<EquipmentSlot, std::string> &getEquipment() const;
    std::string getEquipmentID(EquipmentSlot slot) const;
    /// Return shared pointer to entity in slot, returning nullptr if no entity in slot
    /// \param slot EquipmentSlot to look in
    /// \return shared_ptr to entity in slot
    Entity * getEquipmentEntity(EquipmentSlot slot) const;
    bool equip(EquipmentSlot slot, Entity *entity);
    bool equip(EquipmentSlot slot, std::string ID);
    bool unequip(Entity *entity);
    bool unequip(std::string ID);
    bool unequip(EquipmentSlot slot);
    std::vector<std::string> getInventoryItemsEquippableInSlot(EquipmentSlot slot) const;
    bool hasEquippedInSlot(EquipmentSlot slot) const;
    bool hasEquipped(std::string ID) const;
    EquipmentSlot getEquipmentSlotByID(std::string ID) const;

protected:
    std::unordered_map<std::string, std::unique_ptr<Behaviour>> mBehaviours;
    std::vector<std::string> mInventory;
    Point mPos;
    std::unordered_map<EquipmentSlot, std::string> mEquipment;
    int mMaxCarryWeight;
};

/// Singleton class that manages all entities
class EntityManager {
    std::unordered_map<std::string, std::unique_ptr<Entity>> mEntities;
    std::queue<std::string> mToBeDeleted;
    std::vector<std::string> mCurrentlyOnScreen;
    std::vector<std::string> mInSurroundingScreens;
    std::vector<std::pair<std::string, int>> mToRender;

    Time mTimeOfDay;
    Time mTimePerTick { 0, 2 };

    void reorderEntities();
public:
    static EntityManager& getInstance() {
        static EntityManager instance;
        return instance;
    }

    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    void operator=(const EntityManager&) = delete;

    void addEntity(std::unique_ptr<Entity> entity);
    void broadcast(uint32_t signal);
    void initialize();
    void tick();
    void cleanup(); // Cleanup entities to be deleted

    void render(Font &font, Point currentWorldPos, LightMapTexture &lightMapTexture);
    void render(Font &font, LightMapTexture &lightMapTexture);

    Entity * getEntityByID(const std::string &ID) const;
    void queueForDeletion(const std::string &ID);
    void eraseByID(const std::string &ID);

    /// Find entities at point `pos` WARNING SLOW (reads every entity)
    /// \param pos position to look at
    /// \return vector of shared pointers at pos
    std::vector<Entity *> getEntitiesAtPos(const Point &pos) const;

    /// Find entities at point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look at
    /// \return vector of shared pointers at pos
    std::vector<Entity *> getEntitiesAtPosFaster(const Point &pos) const;

    /// Find entities surrounding point `pos` WARNING SLOW (reads every entity)
    /// \param pos position to look around
    /// \return vector of shared pointers to entities surrounding pos
    std::vector<Entity *> getEntitiesSurrounding(const Point& pos) const;

    /// Find entities surrounding point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look around
    /// \return vector of shared pointers to entities surrounding pos
    std::vector<Entity *> getEntitiesSurroundingFaster(const Point& pos) const;

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