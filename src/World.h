
#ifndef WORLD_H_
#define WORLD_H_

#include "Font.h"
#include "Point.h"
#include <string>
#include <unordered_map>

/// Number of characters of screen width
#define SCREEN_WIDTH 70
/// Number of characters of screen height
#define SCREEN_HEIGHT 35

/// Convert point in world coordinates to screen coordinates
Point worldToScreen(Point worldSpacePoint);

/// Convert coordinates in world grid to absolute world coordinates
Point worldPosToWorld(Point worldPos);

/// This class basically just handles the randomization and drawing of the floor tiles
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