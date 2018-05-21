#include "world.h"
#include "entities.h"
#include "utils.h"
#include <cstdlib>
#include <cmath>
#include <unordered_set>

#define FOR_EACH_SCREEN_POINT for (auto x = 0; x < SCREEN_WIDTH; ++x) \
    for (auto y = 0; y < SCREEN_HEIGHT; ++y)

void World::render(Font &font, const Point worldPos)
{
	if (std::find(generatedScreens.cbegin(), generatedScreens.cend(), worldPos) == generatedScreens.cend())
		randomizeScreensAround(worldPos);

    Color grey = getColor("grey");
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            font.draw(this->floor[worldPosToWorld(worldPos) + Point(x, y)], x, y, grey);
}

void World::randomizeScreensAround(Point pos)
{
    const std::vector<Point> pointsIncludingSurrounding
		{ pos, pos + Point(-1, 0), pos + Point(1, 0), pos + Point(0, -1), pos + Point(0, 1),
          pos + Point(-1, -1), pos + Point(-1, 1), pos + Point(1, -1), pos + Point(1, 1)};

	for (const auto &point : pointsIncludingSurrounding)
	{
		if (std::find(generatedScreens.cbegin(), generatedScreens.cend(), point) == generatedScreens.cend())
			randomizeScreen(point);
	}
}

void World::randomizeScreen(Point worldPos)
{
    auto &manager = EntityManager::getInstance();

	generatedScreens.push_back(worldPos);

	Point p0 = worldPosToWorld(worldPos);
	// On first pass generate floor tiles
    FOR_EACH_SCREEN_POINT
	{
        auto p = p0 + Point(x, y);
        // Generate background tile
        switch (rand() % 4) {
            case 0:
                this->floor[p] = "`";
                break;
            case 1:
                this->floor[p] = "'";
                break;
            case 2:
                this->floor[p] = ".";
                break;
            case 3:
                this->floor[p] = ",";
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
        auto water = std::make_shared<WaterEntity>();
        water->setPos(p);
        manager.addEntity(water);
    }

    // Finally place entities
    FOR_EACH_SCREEN_POINT
    {
        auto p = p0 + Point(x, y);
        // Random chance of creating a bush
        if (randDouble() < 0.002) {
            auto bush = std::make_shared<BushEntity>();
            bush->setPos(p);
            manager.addEntity(bush);
            continue; // Don't put two on same square
        }

        // Random chance of creating a twig
        if (randDouble() < 0.002) {
            auto twig = std::make_shared<TwigEntity>();
            twig->setPos(p);
            manager.addEntity(twig);
            continue;
        }

        // Random chance of creating grass
        if (randDouble() < 0.002) {
            auto grass = std::make_shared<GrassEntity>();
            grass->setPos(p);
            manager.addEntity(grass);
            continue;
        }

        if (randDouble() < 0.0005) {
            auto bug = std::make_shared<GlowbugEntity>();
            bug->setPos(p);
            manager.addEntity(bug);
            continue;
        }

        if (randDouble() < 0.00025) {
            auto wolf = std::make_shared<WolfEntity>();
            wolf->setPos(p);
            manager.addEntity(wolf);
            continue;
        }
	}
}

Point worldToScreen(Point worldSpacePoint) {
    return { worldSpacePoint.x % SCREEN_WIDTH, worldSpacePoint.y % SCREEN_HEIGHT };
}

Point worldPosToWorld(Point worldPos)
{
	return { worldPos.x * SCREEN_WIDTH, worldPos.y * SCREEN_HEIGHT };
}
