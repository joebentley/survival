
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

/// This class handles the randomization and drawing of the floor tiles,
/// as well as the random generation of all entities in the game
struct World {
    /// Each point in the world has a random floor tile glyph
	std::unordered_map<Point, std::string> mFloor;

    void render(Font &font, int worldX, int worldY) {
		render(font, Point(worldX, worldY));
    }
    /// Render the floor tiles at the given world coordinates
    /// \param font the font to render onto
    /// \param worldPos the coordinates on the world grid (each point is a screen)
    void render(Font &font, Point worldPos);

    /// Randomize the screens in each of the eight directions around the screen given by the world coordinates `worldPos`
    /// as well as the screen at `worldPos`
	void randomizeScreensAround(Point worldPos);

	/// Randomize the floor tiles and generate entities for the screen at the world coordinates given by `worldPos`
	void randomizeScreen(Point worldPos);
private:
    /// Keep track of which screens we've generated already
	std::vector<Point> mGeneratedScreens;
};

#endif // WORLD_H_