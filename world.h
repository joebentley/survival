
#ifndef WORLD_H_
#define WORLD_H_

#include "font.h"
#include <string>

#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 35

class World {
public:
    std::string floor[40][80];

    int render(Font& font);
    void randomizeFloor();
};

#endif // WORLD_H_