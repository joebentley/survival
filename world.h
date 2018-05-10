
#ifndef WORLD_H_
#define WORLD_H_

#include "font.h"
#include <string>

#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 35

class World {
public:
    std::string floor[SCREEN_HEIGHT][SCREEN_WIDTH][SCREEN_HEIGHT][SCREEN_WIDTH];

    int render(Font& font, int worldX, int worldY);
    int render(Font& font, std::tuple<int, int> worldPos) {
        return render(font, std::get<0>(worldPos), std::get<1>(worldPos));
    }
    void randomizeFloor();
};

#endif // WORLD_H_