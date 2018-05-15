
#ifndef WORLD_H_
#define WORLD_H_

#include "font.h"
#include "point.h"
#include <string>

#define WORLD_WIDTH 70
#define WORLD_HEIGHT 35
#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 35

Point worldToScreen(Point worldSpacePoint);

struct World {
    std::string floor[WORLD_HEIGHT][WORLD_WIDTH][SCREEN_HEIGHT][SCREEN_WIDTH];

    int render(Font& font, int worldX, int worldY);
    int render(Font& font, Point worldPos) {
        return render(font, worldPos.x, worldPos.y);
    }
    void randomizeWorld();
};

#endif // WORLD_H_