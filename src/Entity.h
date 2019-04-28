
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
#include "World.h"
#include "Point.h"
#include "flags.h"
#include "Time.h"
#include "Texture.h"
#include "Property.h"

/// Tracks the number of initialised entities in the game
extern int gNumInitialisedEntities;

struct Entity;

/// Describes a behaviour that can be attached to an Entity and can update on each tick of the game loop
struct Behaviour {
    /// Construct a new behaviour with given ID and reference to parent (which is stored in the Behaviour)
    Behaviour(std::string ID, Entity& parent) : mID(std::move(ID)), mParent(parent) {}

    virtual ~Behaviour() = default;

    /// Unique ID for the behaviour
    std::string mID;
    /// Mutable reference to the parent
    Entity& mParent;

    /// Called each engine tick, called by the parent entity
    virtual void tick() {};

    /// Handle a specific int signal, not really used right now
    virtual void handle(Uint32 /*signal*/) {};

    /// Is the Behaviour enabled? Can override in subclasses
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

/// Describes the different slots that equipment can be equipped in
enum class EquipmentSlot {
    HEAD,
    TORSO,
    LEGS,
    RIGHT_HAND,
    LEFT_HAND,
    FEET,
    BACK
};

/// Get next equipment slot
EquipmentSlot &operator++(EquipmentSlot &slot);
/// Get previous equipment slot
EquipmentSlot &operator--(EquipmentSlot &slot);

/// Vector of all possible equipment slots (for iteration)
const std::vector<EquipmentSlot> EQUIPMENT_SLOTS { // NOLINT(cert-err58-cpp)
    EquipmentSlot::HEAD, EquipmentSlot::TORSO, EquipmentSlot::LEGS,
    EquipmentSlot::RIGHT_HAND, EquipmentSlot::LEFT_HAND, EquipmentSlot::FEET,
    EquipmentSlot::BACK
};

/// Convert equipment slot to string
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
/// Base entity class for all entities in the game (including player)
struct Entity {
    /// Initialize a new entity
    /// \param ID ID of the entity
    /// \param name descriptive name
    /// \param graphic fontstring to use when rendering
    /// \param hp beginning hp of the entity
    /// \param maxhp maximum hp allowed for the entity
    /// \param regenPerTick amount of hp to regen per tick
    /// \param hitTimes how many times should entity hit per attack
    /// \param hitAmount how much base damage (without considering weapon) should the attack do
    /// \param maxCarryWeight maximum carry weight of entity
    Entity(std::string ID, std::string name, std::string graphic,
           float hp, float maxhp, float regenPerTick, int hitTimes, int hitAmount, int maxCarryWeight)
            : mHp(hp), mMaxHp(maxhp), mRegenPerTick(regenPerTick), mHitTimes(hitTimes), mHitAmount(hitAmount), mID(std::move(ID)),
              mName(std::move(name)), mGraphic(std::move(graphic)), mPos(0, 0), mMaxCarryWeight(maxCarryWeight)
    {
        // If we specify an empty string generate a random ID
        if (this->mID.empty())
            this->mID = std::to_string(rand());
        // Add 1 to number of existing entities
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

    /// Initialize an entity which hits once for damage 2 and has max carry weight 100
    /// \param ID ID of the entity
    /// \param name descriptive name
    /// \param graphic fontstring to use when rendering
    /// \param hp beginning hp of the entity
    /// \param maxhp maximum hp allowed for the entity
    /// \param regenPerTick amount of hp to regen per tick
    Entity(std::string ID, std::string name, std::string graphic, float hp, float maxhp, float regenPerTick)
            : Entity(std::move(ID), std::move(name), std::move(graphic), hp, maxhp, regenPerTick, 1, 2, 100) {}

    /// Initialize an entity with health 1, max health 1, 0 regen per tick, hits once for damage 2, has max carry 100
    /// \param ID ID of the entity
    /// \param name descriptive name
    /// \param graphic fontstring to use when rendering
    Entity(std::string ID, std::string name, std::string graphic)
            : Entity(std::move(ID), std::move(name), std::move(graphic), 1, 1, 0, 1, 2, 100) {}

    virtual ~Entity() = default;

    float mHp; /// Current hp of the entity
    float mMaxHp; /// Maximum hp of the entity
    float mRegenPerTick; /// How much hp to regen per tick
    int mHitTimes; /// How many times should entity hit per attack
    int mHitAmount; /// How much base damage (without considering weapon) should the attack do

    // TODO: next few should certainly be encapsulated
    bool mShouldRender {true}; /// Should we render the entity?
    bool mIsInAnInventory {false}; /// Is the entity currently in an inventory?
    bool mIsEquipped {false}; /// Is the entity currently requipped?
    bool mIsSolid {false}; /// If true, cannot be walked on

    bool mSkipLootingDialog {false}; /// automatically pick up first item in inventory when looting

    std::string mID; /// Unique ID for entity

    float mQuality {1}; /// Quality as a crafting product

    std::string mName; /// Descriptive name
    // TODO: virtual getter for mShortDesc, change based on quality?
    std::string mShortDesc; /// Short one-line description
    std::string mLongDesc; /// Long paragraph description

    std::string mGraphic; /// ASCII graphic text (see Font.h and Font.cpp for text formatting "mini-language")

    int mRenderingLayer {0}; /// Sets render order of the entity

    /// Move the behaviour to the entities vector of behaviours
    virtual void addBehaviour(std::unique_ptr<Behaviour> behaviour);

    /// Regen entity health and tick all Behaviours owned by entity
    virtual void tick();

    /// Remove entity from the entity manager
    virtual void destroy();

    /// Emit signal to all subentities
    virtual void emit(uint32_t signal);

    /// Render the entity to the screen using font, taking current world position into account
    /// \param font to render using
    /// \param currentWorldPos the current screen grid coordinates on the world grid
    virtual void render(Font& font, Point currentWorldPos);

    /// Tests whether the entity is on the current world screen
    /// \param currentWorldPos the current world position of the player (or camera?)
    /// \return whether or not is on screen
    bool isOnScreen(const Point &currentWorldPos);

    /// Handle collision with `pos`, returning true if collision occurred.
    /// Default interpretation is whether `pos == this->getPos()`
    virtual bool collide(const Point &pos);

    /// Handles collision but is also given a reference to the entity that is colliding
    virtual bool collide(const Point &, Entity &) { return false; }

    /// Add entity with ID to inventory
    virtual bool addToInventory(const std::string &ID);
    /// Remove entity with given ID from inventory
    void removeFromInventory(const std::string &ID);
    /// Remove entity from given inventory position
    void removeFromInventory(int inventoryIndex);
    /// Drop entity with given inventory position onto the floor
    void dropItem(int inventoryIndex);
    /// Return pointer to the inventory item at given index
    Entity * getInventoryItem(int inventoryIndex) const;
    /// Get the unique ID of the inventory item at given index
    std::string getInventoryItemID(int inventoryIndex) const;
    /// Get number of items in inventory
    size_t getInventorySize() const;
    /// Is the entities' inventory empty?
    bool isInventoryEmpty() const;
    /// Construct a new vector from the EntityManager containing pointers to all the entities referenced by this entity
    std::vector<Entity *> getInventory() const;
    /// Is a given entity ID in the inventory?
    bool isInInventory(const std::string &ID) const;

    /// Filter inventory for crafting material of type `materialType` returning a vector of ID strings
    /// \param materialType type of material to filter for
    /// \return list of IDs of those materials
    std::vector<std::string> filterInventoryForCraftingMaterial(std::string materialType) const;

    /// Same as `filterInventoryForCraftingMaterial` but match any of the types in `materialTypes`
    /// \param materialTypes types of material to filter for
    /// \return list of IDs of those materials
    std::vector<std::string> filterInventoryForCraftingMaterials(const std::vector<std::string> &materialTypes) const;

    /// Get total weight of all inventory items with "PickuppableProperty"
    int getCarryingWeight();

    /// Set position of the entity to (x, y)
    void setPos(int x, int y) { setPos(Point(x, y)); }
    /// Set position of the entity to p
    void setPos(Point p) { mPos = p; }
    /// Get entity position
    Point getPos() const {
        return mPos;
    }
    /// Get entity position on the world coordinate grid
    Point getWorldPos() const {
        return { this->mPos.mX / SCREEN_WIDTH, this->mPos.mY / SCREEN_HEIGHT };
    }

    /// Get maximum carry weight considering whether a back item is equipped with property "AdditionalCarryWeight"
    int getMaxCarryWeight() const;

    /// Attempt to move to Point P. Will not move to the point if there is a solid entity in the way
    /// \param p Point to move to
    /// \return Whether or not the movement was performed
    bool moveTo(Point p);

    /// Get behaviour with given ID
    Behaviour * getBehaviourByID(const std::string &ID) const;
    /// Does entity have behaviour with given ID?
    bool hasBehaviour(const std::string& ID) const;

    /// Compute max damage considering whether an item with property "MeleeWeaponDamage" is equipped in right hand
    int computeMaxDamage() const;
    /// Calculate a random damage roll
    int rollDamage();

    /// Add some health, not surpassing mMaxHp
    void addHealth(float health);

    /// Get map of all equipment IDs and which slots they are in
    const std::unordered_map<EquipmentSlot, std::string> &getEquipment() const;
    /// Get ID of equipment entity in given slot
    std::string getEquipmentID(EquipmentSlot slot) const;
    /// Return pointer to entity in slot, returning nullptr if no entity in slot
    /// \param slot EquipmentSlot to look in
    /// \return pointer to entity in slot
    Entity * getEquipmentEntity(EquipmentSlot slot) const;
    /// Equip given entity in slot, returning true if successful
    bool equip(EquipmentSlot slot, Entity *entity);
    /// Equip entity with ID in slot, returning true if successful
    bool equip(EquipmentSlot slot, const std::string &ID);
    /// Unequip given entity, returning true if it was equipped
    bool unequip(Entity *entity);
    /// Unequip entity with given ID, returning true if it was equipped
    bool unequip(const std::string &ID);
    /// Unequip entity from slot, returning true if it was equipped
    bool unequip(EquipmentSlot slot);
    /// Get vector of entity IDs from inventory that are equippable in the given slot
    std::vector<std::string> getInventoryItemsEquippableInSlot(EquipmentSlot slot) const;
    /// Is there anything equipped in the given slot?
    bool hasEquippedInSlot(EquipmentSlot slot) const;
    /// Is the entity given by ID currently equipped?
    bool hasEquipped(const std::string &ID) const;
    /// Get the slot that the entity with ID is equipped in, throwing std::out_of_range if it is not equpiped
    EquipmentSlot getEquipmentSlotByID(const std::string &ID) const;

    /// Does the entity have the given property?
    bool hasProperty(const std::string &propertyName) const;
    /// Return a pointer to the property object with propertyName
    Property * getProperty(const std::string &propertyName) const;
    /// Move the given property to the entity
    void addProperty(std::unique_ptr<Property> property);

protected:
    /// Map of behaviour IDs to unique pointers owning those Behaviours
    std::unordered_map<std::string, std::unique_ptr<Behaviour>> mBehaviours;
    /// Map of property IDs to unique pointers owning those Properties
    std::unordered_map<std::string, std::unique_ptr<Property>> mProperties;
    /// Vector of IDs of entities in this entity's inventory
    std::vector<std::string> mInventory;
    /// Absolute grid position of the entity
    Point mPos;
    /// Map from equipment slot to entity ID (of the item equipped in that slot)
    std::unordered_map<EquipmentSlot, std::string> mEquipment;
    /// Maximum carry weight of entity
    int mMaxCarryWeight;
};

/// Singleton class that manages all entities in the game
class EntityManager {
    /// Map from the entity ID to a unique pointer owning the Entity instance
    std::unordered_map<std::string, std::unique_ptr<Entity>> mEntities;
    /// Queue of entity IDs to be released
    std::queue<std::string> mToBeDeleted;
    /// Vector of entity IDs that are currently on the screen
    std::vector<std::string> mCurrentlyOnScreen;
    /// Vector of entity IDs that are on surrounding screens
    std::vector<std::string> mInSurroundingScreens;
    /// Vector of pairs of entity IDs to be rendered with their render ordering as ints
    std::vector<std::pair<std::string, int>> mToRender;

    /// Current time of day the game
    Time mTimeOfDay;
    /// Amount of time to increment per game tick
    Time mTimePerTick { 0, 2 };

    /// Generate mToRender from the entities given by mCurrentlyOnScreen in the order given by their mRenderingLayer fields
    void reorderEntities();
public:
    /// Get the singleton instance
    static EntityManager& getInstance() {
        static EntityManager instance;
        return instance;
    }

    EntityManager() = default;
    // Singleton, so delete copy constructor and copy assignment operator
    EntityManager(const EntityManager&) = delete;
    void operator=(const EntityManager&) = delete;

    /// Move the entity to be managed by the EntityManager, throwing an std::invalid_argument exception if entity with ID is already present
    void addEntity(std::unique_ptr<Entity> entity);
    /// Broadcast the signal to all entities, which emit them to all behaviours
    void broadcast(uint32_t signal);
    /// Checks if the number of entities in the entity manager equals the count of the number of entities initialized so far,
    /// printing a warning if they are not equal. Then calls recomputeCurrentEntitiesOnScreenAndSurroundingScreens with the
    /// player's world position
    void initialize();
    /// cleanup() and then advance the game time, then tick all entities on this screen or the surrounding screens
    void tick();
    /// Remove the entities in mToBeDeleted
    void cleanup();

    /// Render the world and all entities to the font using the currentWorldPos,
    /// also rendering all light sources using LightMapTexture with alpha
    /// depending on time of day
    void render(Font &font, Point currentWorldPos, LightMapTexture &lightMapTexture);
    /// Same as other but uses the player's world position as currentWorldPos
    void render(Font &font, LightMapTexture &lightMapTexture);

    /// Get pointer to entity with ID, nullptr if it doesn't exist
    Entity * getEntityByID(const std::string &ID) const;
    /// Queue entity with ID for deletion on next cleanup()
    void queueForDeletion(const std::string &ID);
    /// Erase the entity with ID, also --gNumInitialisedEntities, and calls recomputeCurrentEntitiesOnScreenAndSurroundingScreens
    /// with the player's world position
    void eraseByID(const std::string &ID);

    /// Find entities at point `pos` WARNING SLOW (reads every entity in world)
    /// \param pos position to look at
    /// \return vector of pointers to entities at pos
    std::vector<Entity *> getEntitiesAtPos(const Point &pos) const;

    /// Find entities at point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look at
    /// \return vector of pointers to entities at pos
    std::vector<Entity *> getEntitiesAtPosFaster(const Point &pos) const;

    /// Find entities surrounding point `pos` WARNING SLOW (reads every entity)
    /// \param pos position to look around
    /// \return vector of pointers to entities surrounding pos
    std::vector<Entity *> getEntitiesSurrounding(const Point& pos) const;

    /// Find entities surrounding point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look around
    /// \return vector of pointers to entities surrounding pos
    std::vector<Entity *> getEntitiesSurroundingFaster(const Point& pos) const;

    /// Union of mCurrentlyOnScreen and mInSurroundingScreens
    /// \return vector of pointers to entities
    std::vector<Entity *> getEntitiesOnScreenAndSurroundingScreens() const;

    /// Call collision(pos) for all entities in current and surrounding screens
    /// \param pos the new position of the entity that is trying to move
    /// \param entity a reference to the entity that is trying to move
    /// \return vector of pointers to all entities that collided
    std::vector<Entity *> doCollisions(const Point& pos, Entity &entity);

    /// Is entity with ID registered in the manager?
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