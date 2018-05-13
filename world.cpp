#include "world.h"
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

void World::randomizeFloor()
{
    for (int worldY = 0; worldY < SCREEN_HEIGHT; ++worldY)
    for (int worldX = 0; worldX < SCREEN_WIDTH; ++worldX)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
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
    }
}

Point worldToScreen(Point worldSpacePoint) {
    return Point(worldSpacePoint.x % SCREEN_WIDTH, worldSpacePoint.y % SCREEN_HEIGHT);
}
