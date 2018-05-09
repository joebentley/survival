
#ifndef WORLD_H_
#define WORLD_H_

#include "font.h"
#include <string>

#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 35

class World {
public:
    std::string floor[SCREEN_HEIGHT][SCREEN_WIDTH];

    int render(Font& font);
    void randomizeFloor();
};

#endif // WORLD_H_