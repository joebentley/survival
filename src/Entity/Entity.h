
#ifndef ENTITY_H_
#define ENTITY_H_

#include "../Behaviour/Behaviour.h"
#include "../Point.h"
#include "../Property.h"
#include "EquipmentSlot.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/// Tracks the number of initialised entities in the game
extern int gNumInitialisedEntities;

class Font;
struct World;
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
    Entity(std::string ID, std::string name, std::string graphic, float hp, float maxhp, float regenPerTick,
           int hitTimes, int hitAmount, int maxCarryWeight);

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

    float mHp;           /// Current hp of the entity
    float mMaxHp;        /// Maximum hp of the entity
    float mRegenPerTick; /// How much hp to regen per tick
    int mHitTimes;       /// How many times should entity hit per attack
    int mHitAmount;      /// How much base damage (without considering weapon) should the attack do

    // TODO: next few should certainly be encapsulated
    bool mShouldRender{true};     /// Should we render the entity?
    bool mIsInAnInventory{false}; /// Is the entity currently in an inventory?
    bool mIsEquipped{false};      /// Is the entity currently requipped?
    bool mIsSolid{false};         /// If true, cannot be walked on

    bool mSkipLootingDialog{false}; /// automatically pick up first item in inventory when looting

    std::string mID; /// Unique ID for entity

    float mQuality{1}; /// Quality as a crafting product

    std::string mName; /// Descriptive name
    // TODO: virtual getter for mShortDesc, change based on quality?
    std::string mShortDesc; /// Short one-line description
    std::string mLongDesc;  /// Long paragraph description

    std::string mGraphic; /// ASCII graphic text (see Font.h and Font.cpp for text formatting "mini-language")

    int mRenderingLayer{0}; /// Sets render order of the entity

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
    virtual void render(Font &font, Point currentWorldPos);

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
    Entity *getInventoryItem(int inventoryIndex) const;
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
    void setPos(int x, int y);
    /// Set position of the entity to p
    void setPos(Point p);
    /// Get entity position
    Point getPos() const;
    /// Get entity position on the world coordinate grid
    Point getWorldPos() const;

    /// Get maximum carry weight considering whether a back item is equipped with property "AdditionalCarryWeight"
    int getMaxCarryWeight() const;

    /// Attempt to move to Point p, also moving the entity's inventory, and recomputing the current screen's entities if
    /// it changes screen coords (TODO: do we need to do this if it's not the player's movement?). Will not move to the
    /// point if there is a solid entity in the way
    /// \param p Point to move to
    /// \return Whether or not the movement was performed
    bool moveTo(Point p);

    /// Get behaviour with given ID
    Behaviour *getBehaviourByID(const std::string &ID) const;
    /// Does entity have behaviour with given ID?
    bool hasBehaviour(const std::string &ID) const;
    /// Disable any Wander and WanderAttach behaviours
    void disableWanderBehaviours();
    /// Enable any Wander and WanderAttach behaviours
    void enableWanderBehaviours();
    // TODO: function to disable (and enable) behaviours with given IDs

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
    Entity *getEquipmentEntity(EquipmentSlot slot) const;
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
    Property *getProperty(const std::string &propertyName) const;
    /// Move the given property to the entity
    void addProperty(std::unique_ptr<Property> property);

    bool canBeAttacked() const { return mCanBeAttacked; }

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
    /// Whether or not the entity can be attacked
    bool mCanBeAttacked = true;
};
#endif // ENTITY_H_
