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

    for (int worldY = 0; worldY < SCREEN_HEIGHT; ++worldY)
    for (int worldX = 0; worldX < SCREEN_WIDTH; ++worldX)
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
        if (randFloat() < 0.002) {
            auto bush = std::make_shared<BushEntity>("bush" + std::to_string(++numBushes));
            bush->setPos(p);
            manager.addEntity(bush);
        }

        // Random chance of creating a twig
        if (randFloat() < 0.002) {
            auto twig = std::make_shared<TwigEntity>("twig" + std::to_string(++numTwigs));
            twig->setPos(p);
            manager.addEntity(twig);
        }
    }
}

Point worldToScreen(Point worldSpacePoint) {
    return { worldSpacePoint.x % SCREEN_WIDTH, worldSpacePoint.y % SCREEN_HEIGHT };
}