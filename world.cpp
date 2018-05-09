#include "world.h"
#include <algorithm>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

int World::render(Font& font)
{
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (font.drawText(this->floor[y][x], x, y) == -1)
                return -1;
        }
    }
    return 0;
}

void World::randomizeFloor()
{
    srand(time(NULL));

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            this->floor[y][x] = "$[grey]";
            switch (rand() % 4) {
                case 0:
                    this->floor[y][x] += "`";
                    break;
                case 1:
                    this->floor[y][x] += "'";
                    break;
                case 2:
                    this->floor[y][x] += ".";
                    break;
                case 3:
                    this->floor[y][x] += ",";
                    break;
            }
        }
    }
}