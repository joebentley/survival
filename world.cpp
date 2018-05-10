#include "world.h"
#include <algorithm>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

int World::render(Font& font, int worldX, int worldY)
{
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            if (font.drawText(this->floor[worldY][worldX][y][x], x, y) == -1)
                return -1;
    
    return 0;
}

void World::randomizeFloor()
{
    for (int worldY = 0; worldY < SCREEN_HEIGHT; ++worldY)
    for (int worldX = 0; worldX < SCREEN_WIDTH; ++worldX)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        this->floor[worldY][worldX][y][x] = "$[grey]";
        switch (rand() % 4) {
            case 0:
                this->floor[worldY][worldX][y][x] += "`";
                break;
            case 1:
                this->floor[worldY][worldX][y][x] += "'";
                break;
            case 2:
                this->floor[worldY][worldX][y][x] += ".";
                break;
            case 3:
                this->floor[worldY][worldX][y][x] += ",";
                break;
        }
    }
}