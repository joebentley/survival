
#ifndef WORLD_H_
#define WORLD_H_

#include "font.h"
#include "point.h"
#include <string>

#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 35

Point worldToScreen(Point worldSpacePoint);

class World {
public:
    std::string floor[SCREEN_HEIGHT][SCREEN_WIDTH][SCREEN_HEIGHT][SCREEN_WIDTH];

    int render(Font& font, int worldX, int worldY);
    int render(Font& font, Point worldPos) {
        return render(font, worldPos.x, worldPos.y);
    }
    void randomizeWorld();
};

#endif // WORLD_H_