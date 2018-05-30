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

    Color grey = FontColor::getColor("grey");
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            font.draw(this->mFloor[worldPosToWorld(worldPos) + Point(x, y)], x, y, grey);
}

void World::randomizeScreensAround(Point pos)
{
    const std::vector<Point> pointsIncludingSurrounding
		{ pos, pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
          pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

	for (const auto &point : pointsIncludingSurrounding)
	{
		if (std::find(mGeneratedScreens.cbegin(), mGeneratedScreens.cend(), point) == mGeneratedScreens.cend())
			randomizeScreen(point);
	}
}

void World::randomizeScreen(Point worldPos)
{
    auto &manager = EntityManager::getInstance();

	mGeneratedScreens.push_back(worldPos);

	Point p0 = worldPosToWorld(worldPos);
	// On first pass generate floor tiles
    FOR_EACH_SCREEN_POINT
	{
        auto p = p0 + Point(x, y);
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

    // Choose random points from which to originate pools of water
    const int numPools = rand() % 3;
	std::vector<Point> poolOriginCoords;
	for (auto i = 0; i < numPools; ++i)
	    poolOriginCoords.emplace_back(p0 + Point(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT));

	// Set of tiles that will have water in (used set to avoid duplicates)
    std::unordered_set<Point> currentWaterTiles;
    for (auto poolOrigin : poolOriginCoords)
        currentWaterTiles.emplace(poolOrigin);

    // Generate water pools scaling probability on manhattan distance from pool
    FOR_EACH_SCREEN_POINT
    {
        auto p = p0 + Point(x, y);

        for (auto poolOrigin : poolOriginCoords)
            if (randDouble() < std::exp(-std::pow(p.manhattanDistanceTo(poolOrigin) / 2.5, 2)))
                currentWaterTiles.emplace(p);
    }

    // Add all the generated water tiles
    for (auto p : currentWaterTiles) {
        auto water = std::make_unique<WaterEntity>();
        water->setPos(p);
        manager.addEntity(std::move(water));
    }

    // Finally place entities
    FOR_EACH_SCREEN_POINT
    {
        auto p = p0 + Point(x, y);
        // Random chance of creating a bush
        if (randDouble() < 0.002) {
            auto bush = std::make_unique<BushEntity>();
            bush->setPos(p);
            manager.addEntity(std::move(bush));
            continue; // Don't put two on same square
        }

        // Random chance of creating a twig
        if (randDouble() < 0.002) {
            auto twig = std::make_unique<TwigEntity>();
            twig->setPos(p);
            manager.addEntity(std::move(twig));
            continue;
        }

        // Random chance of creating grass
        if (randDouble() < 0.002) {
            auto grass = std::make_unique<GrassEntity>();
            grass->setPos(p);
            manager.addEntity(std::move(grass));
            continue;
        }

        if (randDouble() < 0.0005) {
            auto bug = std::make_unique<GlowbugEntity>();
            bug->setPos(p);
            manager.addEntity(std::move(bug));
            continue;
        }

        if (randDouble() < 0.00025) {
            auto wolf = std::make_unique<WolfEntity>();
            wolf->setPos(p);
            manager.addEntity(std::move(wolf));
            continue;
        }
	}
}

Point worldToScreen(Point worldSpacePoint) {
    return { worldSpacePoint.mX % SCREEN_WIDTH, worldSpacePoint.mY % SCREEN_HEIGHT };
}

Point worldPosToWorld(Point worldPos) {
	return { worldPos.mX * SCREEN_WIDTH, worldPos.mY * SCREEN_HEIGHT };
}