#pragma once

#include "../Time.h"
#include "Entity.h"

#include <queue>
#include <unordered_map>
#include <vector>

struct LightMapPoint;
class LightMapTexture;
/// Singleton class that manages all entities in the game
class EntityManager {
    /// Map from the entity ID to a unique pointer owning the Entity instance
    std::unordered_map<std::string, std::unique_ptr<Entity>> mEntities{};
    /// Queue of entity IDs to be released
    std::queue<std::string> mToBeDeleted{};
    /// Vector of entity IDs that are currently on the screen
    std::vector<std::string> mCurrentlyOnScreen{};
    /// Vector of entity IDs that are on surrounding screens
    std::vector<std::string> mInSurroundingScreens{};
    /// Vector of pairs of entity IDs to be rendered with their render ordering as ints
    std::vector<std::pair<std::string, int>> mToRender{};

    /// Current time of day the game
    Time mTimeOfDay{};
    /// Amount of time to increment per game tick
    Time mTimePerTick{0, 2};

    /// Generate mToRender from the entities given by mCurrentlyOnScreen in the order given by their mRenderingLayer
    /// fields
    void reorderEntities();

  public:
    /// Get the singleton instance
    static EntityManager &getInstance() {
        static EntityManager instance;
        return instance;
    }

    EntityManager() = default;
    // Singleton, so delete copy constructor and copy assignment operator
    EntityManager(const EntityManager &) = delete;
    void operator=(const EntityManager &) = delete;

    /// Move the entity to be managed by the EntityManager, throwing an std::invalid_argument exception if entity with
    /// ID is already present
    void addEntity(std::unique_ptr<Entity> entity);
    /// Broadcast the signal to all entities, which emit them to all behaviours
    void broadcast(uint32_t signal);
    /// Checks if the number of entities in the entity manager equals the count of the number of entities initialized so
    /// far, printing a warning if they are not equal. Then calls recomputeCurrentEntitiesOnScreenAndSurroundingScreens
    /// with the player's world position
    void initialize();
    /// cleanup() and then advance the game time, then tick all entities on this screen or the surrounding screens
    /// (relative to the player)
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
    Entity *getEntityByID(const std::string &ID) const;
    /// Queue entity with ID for deletion on next cleanup()
    void queueForDeletion(const std::string &ID);
    /// Erase the entity with ID, also --gNumInitialisedEntities, and calls
    /// recomputeCurrentEntitiesOnScreenAndSurroundingScreens with the player's world position
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
    std::vector<Entity *> getEntitiesSurrounding(const Point &pos) const;

    /// Find entities surrounding point `pos` searching only entities on the surrounding screens of player
    /// \param pos position to look around
    /// \return vector of pointers to entities surrounding pos
    std::vector<Entity *> getEntitiesSurroundingFaster(const Point &pos) const;

    /// Union of mCurrentlyOnScreen and mInSurroundingScreens
    /// \return vector of pointers to entities
    std::vector<Entity *> getEntitiesOnScreenAndSurroundingScreens() const;

    /// Call collision(pos) for all entities in current and surrounding screens
    /// \param pos the new position of the entity that is trying to move
    /// \param entity a reference to the entity that is trying to move
    /// \return vector of pointers to all entities that collided
    std::vector<Entity *> doCollisions(const Point &pos, Entity &entity);

    /// Is entity with ID registered in the manager?
    bool isEntityInManager(const std::string &ID);

    /// Should be called every time the player changes screen. Recomputes current entities on this screen and
    /// surrounding screens
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
