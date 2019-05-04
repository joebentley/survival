#include "World.h"
#include "Entities.h"
#include "utils.h"
#include <cstdlib>
#include <cmath>
#include <unordered_set>

#define FOR_EACH_SCREEN_POINT for (auto x = 0; x < SCREEN_WIDTH; ++x) \
    for (auto y = 0; y < SCREEN_HEIGHT; ++y)

void World::render(Font &font, const Point worldPos)
{
	if (std::find(mGeneratedScreens.cbegin(), mGeneratedScreens.cend(), worldPos) == mGeneratedScreens.cend())
		randomizeScreensAround(worldPos);

    Color grey = FontColor::getColor("grassgreen");
    FOR_EACH_SCREEN_POINT
        font.draw(this->mFloor[worldPosToWorld(worldPos) + Point(x, y)], x, y, grey);
}

void World::randomizeScreensAround(Point pos)
{
    const std::vector<Point> pointsIncludingSurrounding
		{ pos, pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
          pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

	for (const auto &point : pointsIncludingSurrounding)
	{
	    // if this screen has not already been generated
		if (std::find(mGeneratedScreens.cbegin(), mGeneratedScreens.cend(), point) == mGeneratedScreens.cend())
			randomizeScreen(point);
	}
}

void World::randomizeScreen(Point worldPos)
{
    auto &manager = EntityManager::getInstance();

    // keep track of the fact we've generated this screen
	mGeneratedScreens.push_back(worldPos);

	// The top-left origin of the screen in world coordinates
	Point p0 = worldPosToWorld(worldPos);

	// On first pass generate floor tiles
    FOR_EACH_SCREEN_POINT
	{
        Point p = p0 + Point(x, y);
        // Generate background tile
        switch (rand() % 4) {
            case 0:
                this->mFloor[p] = "`";
                break;
            case 1:
                this->mFloor[p] = "'";
                break;
            case 2:
                this->mFloor[p] = ".";
                break;
            case 3:
                this->mFloor[p] = ",";
                break;
        }
    }

    /// Generate random pools
    const int numPools = rand() % 3;
	std::vector<Point> poolOriginCoords;
	poolOriginCoords.reserve(numPools);

    // Choose random points from which to originate pools of water
	for (int i = 0; i < numPools; ++i)
	    poolOriginCoords.emplace_back(p0 + Point(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT));

	// Set of tiles that will have water in (used set to avoid duplicates)
    std::unordered_set<Point> currentWaterTiles;
    for (Point poolOrigin : poolOriginCoords)
        currentWaterTiles.emplace(poolOrigin);

    // Generate water pools scaling probability on manhattan distance from pool origin
    FOR_EACH_SCREEN_POINT
    {
        Point p = p0 + Point(x, y);

        for (Point poolOrigin : poolOriginCoords)
            if (randDouble() < std::exp(-std::pow(p.manhattanDistanceTo(poolOrigin) / 2.5, 2)))
                currentWaterTiles.emplace(p);
    }

    // Add all the generated water tiles as entities
    for (Point p : currentWaterTiles) {
        auto water = std::make_unique<WaterEntity>();
        water->setPos(p);
        manager.addEntity(std::move(water));
    }

    /// Place other random entities
    FOR_EACH_SCREEN_POINT
    {
        Point p = p0 + Point(x, y);
        // Random chance of creating a bush
        if (randDouble() < 0.002) {
            auto bush = std::make_unique<BushEntity>();
            bush->setPos(p);
            manager.addEntity(std::move(bush));
        }
        // Random chance of creating a twig
        else if (randDouble() < 0.002) {
            auto twig = std::make_unique<TwigEntity>();
            twig->setPos(p);
            manager.addEntity(std::move(twig));
        }
        // Random chance of creating grass
        else if (randDouble() < 0.002) {
            auto grass = std::make_unique<GrassEntity>();
            grass->setPos(p);
            manager.addEntity(std::move(grass));
        }
        else if (randDouble() < 0.0005) {
            auto bug = std::make_unique<GlowbugEntity>();
            bug->setPos(p);
            manager.addEntity(std::move(bug));
        }
        else if (randDouble() < 0.0001) {
            auto wolf = std::make_unique<WolfEntity>();
            wolf->setPos(p);
            manager.addEntity(std::move(wolf));
        }
        else if (randDouble() < 0.001) {
            auto bunny = std::make_unique<BunnyEntity>();
            bunny->setPos(p);
            manager.addEntity(std::move(bunny));
        }
	}
}

Point worldToScreen(Point worldSpacePoint) {
    return { worldSpacePoint.mX % SCREEN_WIDTH, worldSpacePoint.mY % SCREEN_HEIGHT };
}

Point worldPosToWorld(Point worldPos) {
	return { worldPos.mX * SCREEN_WIDTH, worldPos.mY * SCREEN_HEIGHT };
}
