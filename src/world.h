
#ifndef WORLD_H_
#define WORLD_H_

#include "font.h"
#include "point.h"
#include <string>
#include <unordered_map>

#define SCREEN_WIDTH 70
#define SCREEN_HEIGHT 35

Point worldToScreen(Point worldSpacePoint);
Point worldPosToWorld(Point worldPos);

struct World {
	std::unordered_map<Point, std::string> mFloor;

    void render(Font &font, int worldX, int worldY) {
		render(font, Point(worldX, worldY));
    }
    void render(Font &font, Point worldPos);

	void randomizeScreensAround(Point worldPos);
	void randomizeScreen(Point worldPos);
private:
	std::vector<Point> mGeneratedScreens;
};

#endif // WORLD_H_