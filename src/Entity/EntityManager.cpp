#include "EntityManager.h"

#include "../Font.h"
#include "../LightMapPoint.h"
#include "../LightMapTexture.h"
#include "../Properties.h"
#include "../World.h"
#include <iostream>

void EntityManager::addEntity(std::unique_ptr<Entity> entity) {
    if (getEntityByID(entity->mID) != nullptr)
        throw std::invalid_argument("Entity with ID " + entity->mID + " already present!");

    mEntities[entity->mID] = std::move(entity);

    // Make sure new entities trigger a refresh of the render order list and entity caches
    if (!mToRender.empty())
        recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::broadcast(Uint32 signal) {
    for (const auto &entity : mEntities) {
        entity.second->emit(signal);
    }
}

#define UNMANAGED_ENTITIES_ERROR_MESSAGE                                                                               \
    "Warning! The number of initialised entities is not equal to the number of entities in the entity manager!"

void EntityManager::initialize() {
    if ((size_t)gNumInitialisedEntities != mEntities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::tick() {
    cleanup();

    mTimeOfDay += mTimePerTick;

    if ((size_t)gNumInitialisedEntities != mEntities.size())
        std::cerr << UNMANAGED_ENTITIES_ERROR_MESSAGE << std::endl;

    // Only update entities in this screen and surrounding screens
    // TODO: add special type of entity that always keeps updated e.g. the player's farm
    for (const auto &ID : mCurrentlyOnScreen)
        getEntityByID(ID)->tick();
    for (const auto &ID : mInSurroundingScreens)
        getEntityByID(ID)->tick();
}

void EntityManager::render(Font &font, Point currentWorldPos, LightMapTexture &lightMapTexture) {
    for (const auto &a : mToRender) {
        getEntityByID(a.first)->render(font, currentWorldPos);
    }

    // Draw time-of-day fog
    auto frac = EntityManager::getInstance().getTimeOfDay().getFractionOfDay();
    auto a = 0.6 + 0.8 * std::sin(2 * M_PI * frac - M_PI / 2);
    if (a < 0)
        a = 0;
    else if (a > 1)
        a = 1;
    auto alpha = static_cast<Uint8>(a * 0xFF);
    lightMapTexture.render(getLightSources(font.getCellSize()), alpha);
}

void EntityManager::render(Font &font, LightMapTexture &lightMapTexture) {
    render(font, getEntityByID("Player")->getWorldPos(), lightMapTexture);
}

Entity *EntityManager::getEntityByID(const std::string &ID) const {
    if (mEntities.find(ID) == mEntities.cend())
        return nullptr;
    return mEntities.at(ID).get();
}

std::vector<Entity *> EntityManager::getEntitiesAtPos(const Point &pos) const {
    std::vector<Entity *> entitiesAtPos;

    for (const auto &entity : mEntities) {
        if (entity.second->getPos() == pos)
            entitiesAtPos.push_back(entity.second.get());
    }

    return entitiesAtPos;
}

std::vector<Entity *> EntityManager::getEntitiesAtPosFaster(const Point &pos) const {
    std::vector<Entity *> entitiesAtPos;

    // TODO: remove ugly double loop
    for (const auto &ID : mInSurroundingScreens) {
        if (getEntityByID(ID)->getPos() == pos)
            entitiesAtPos.push_back(getEntityByID(ID));
    }
    for (const auto &ID : mCurrentlyOnScreen) {
        if (getEntityByID(ID)->getPos() == pos)
            entitiesAtPos.push_back(getEntityByID(ID));
    }

    return entitiesAtPos;
}

std::vector<Entity *> EntityManager::getEntitiesSurrounding(const Point &pos) const {
    std::vector<Entity *> entitiesSurrounding;
    const std::vector<Point> surroundingPoints{pos + Point(-1, 0), pos + Point(1, 0),   pos + Point(0, -1),
                                               pos + Point(0, 1),  pos + Point(-1, -1), pos + Point(-1, 1),
                                               pos + Point(1, -1), pos + Point(1, 1)};

    for (const auto &entity : mEntities) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), entity.second->getPos()) !=
            surroundingPoints.cend())
            entitiesSurrounding.emplace_back(entity.second.get());
    }

    return entitiesSurrounding;
}

std::vector<Entity *> EntityManager::getEntitiesSurroundingFaster(const Point &pos) const {
    std::vector<Entity *> entitiesSurrounding;
    const std::vector<Point> surroundingPoints{pos + Point(-1, 0), pos + Point(1, 0),   pos + Point(0, -1),
                                               pos + Point(0, 1),  pos + Point(-1, -1), pos + Point(-1, 1),
                                               pos + Point(1, -1), pos + Point(1, 1)};

    // TODO: remove ugly double loop
    for (const auto &ID : mInSurroundingScreens) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), getEntityByID(ID)->getPos()) !=
            surroundingPoints.cend())
            entitiesSurrounding.emplace_back(getEntityByID(ID));
    }
    for (const auto &ID : mCurrentlyOnScreen) {
        if (std::find(surroundingPoints.cbegin(), surroundingPoints.cend(), getEntityByID(ID)->getPos()) !=
            surroundingPoints.cend())
            entitiesSurrounding.emplace_back(getEntityByID(ID));
    }

    return entitiesSurrounding;
}

std::vector<Entity *> EntityManager::getEntitiesOnScreenAndSurroundingScreens() const {
    std::vector<Entity *> entities;
    for (const auto &ID : mCurrentlyOnScreen) {
        entities.push_back(getEntityByID(ID));
    }
    for (const auto &ID : mInSurroundingScreens) {
        entities.push_back(getEntityByID(ID));
    }
    return entities;
}

std::vector<Entity *> EntityManager::doCollisions(const Point &pos, Entity &entity) {
    std::vector<Entity *> collidingEntities;
    for (const auto &ID : mCurrentlyOnScreen) {
        Entity *e = getEntityByID(ID);
        // execute the entity's collision, which returns true if a collision occurred
        if (e->collide(pos) || e->collide(pos, entity))
            collidingEntities.push_back(e);
    }
    for (const auto &ID : mInSurroundingScreens) {
        Entity *e = getEntityByID(ID);
        if (e->collide(pos) || e->collide(pos, entity))
            collidingEntities.push_back(e);
    }
    return collidingEntities;
}

void EntityManager::cleanup() {
    while (!mToBeDeleted.empty()) {
        eraseByID(mToBeDeleted.front());
        mToBeDeleted.pop();
    }
}

void EntityManager::queueForDeletion(const std::string &ID) { mToBeDeleted.push(ID); }

void EntityManager::eraseByID(const std::string &ID) {
    mEntities.erase(ID);
    --gNumInitialisedEntities;
    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}

void EntityManager::reorderEntities() {
    mToRender.clear();
    // Make pairs of entity IDs with their rendering layers
    std::transform(
        mCurrentlyOnScreen.cbegin(), mCurrentlyOnScreen.cend(), std::back_inserter(mToRender),
        [this](const std::string &ID) -> auto { return std::make_pair(ID, getEntityByID(ID)->mRenderingLayer); });
    std::sort(mToRender.begin(), mToRender.end(), [](auto &a, auto &b) { return a.second > b.second; });
}

bool EntityManager::isEntityInManager(const std::string &ID) { return mEntities.find(ID) != mEntities.end(); }

// TODO should split this into two separate functions for current entities on screen and for surrounding screens
// player should call both current screen and surrounding screens, but other entity should only call current screen
void EntityManager::recomputeCurrentEntitiesOnScreenAndSurroundingScreens(Point currentWorldPos) {
    mCurrentlyOnScreen.clear();
    mInSurroundingScreens.clear();
    for (const auto &a : mEntities) {
        auto worldPosDiff = a.second->getWorldPos() - currentWorldPos;
        if (worldPosDiff == Point(0, 0))
            mCurrentlyOnScreen.emplace_back(a.second->mID);
        else if (std::abs(worldPosDiff.mX) <= 1 && std::abs(worldPosDiff.mY) <= 1)
            mInSurroundingScreens.emplace_back(a.second->mID);
    }
    reorderEntities();
}

const Time &EntityManager::getTimeOfDay() const { return mTimeOfDay; }

void EntityManager::setTimeOfDay(const Time &timeOfDay) { EntityManager::mTimeOfDay = timeOfDay; }

const Time &EntityManager::getTimePerTick() const { return mTimePerTick; }

void EntityManager::setTimePerTick(const Time &timePerTick) { EntityManager::mTimePerTick = timePerTick; }

std::vector<LightMapPoint> EntityManager::getLightSources(Point fontSize) const {
    std::vector<LightMapPoint> points;

    for (const auto &a : mCurrentlyOnScreen) {
        const auto &entity = getEntityByID(a);
        auto b = entity->getProperty("LightEmitting");
        if (b != nullptr) {
            auto light = std::any_cast<LightEmittingProperty::Light>(b->getValue());
            if (light.isEnabled()) {
                auto radius = fontSize.mY * light.getRadius();
                auto point = fontSize * World::worldToScreen(entity->getPos()) + fontSize / 2;
                points.emplace_back(LightMapPoint(point, radius, light.getColor()));
            }
        }
    }

    return points;
}

void EntityManager::recomputeCurrentEntitiesOnScreenAndSurroundingScreens() {
    recomputeCurrentEntitiesOnScreenAndSurroundingScreens(getEntityByID("Player")->getWorldPos());
}