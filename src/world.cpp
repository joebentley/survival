#include "world.h"
#include "entities.h"
#include "utils.h"
#include <cstdlib>
#include <cstdio>

int World::render(Font& font, int worldX, int worldY)
{
    Color grey = getColor("grey");
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            if (font.draw(this->floor[worldY][worldX][y][x], x, y, grey) == -1)
                return -1;
    return 0;
}

void World::randomizeWorld()
{
    auto &manager = EntityManager::getInstance();
    int numBushes = 0;
    int numTwigs = 0;
    int numGrass = 0;

    for (int worldY = 0; worldY < WORLD_HEIGHT; ++worldY)
    for (int worldX = 0; worldX < WORLD_WIDTH; ++worldX)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        // Generate background tile
        switch (rand() % 4) {
            case 0:
                this->floor[worldY][worldX][y][x] = "`";
                break;
            case 1:
                this->floor[worldY][worldX][y][x] = "'";
                break;
            case 2:
                this->floor[worldY][worldX][y][x] = ".";
                break;
            case 3:
                this->floor[worldY][worldX][y][x] = ",";
                break;
        }

        Point p(worldX * SCREEN_WIDTH + x, worldY * SCREEN_HEIGHT + y);

        // Random chance of creating a bush
        if (randDouble() < 0.002) {
            auto bush = std::make_shared<BushEntity>("bush" + std::to_string(++numBushes));
            bush->setPos(p);
            manager.addEntity(bush);
            continue; // Don't put two on same square
        }

        // Random chance of creating a twig
        if (randDouble() < 0.002) {
            auto twig = std::make_shared<TwigEntity>("twig" + std::to_string(++numTwigs));
            twig->setPos(p);
            manager.addEntity(twig);
            continue;
        }

        // Random chance of creating grass
        if (randDouble() < 0.002) {
            auto grass = std::make_shared<GrassEntity>("grass" + std::to_string(++numGrass));
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
