#pragma once

#include "../Entity.h"
#include <unordered_set>

/// Generates a set of walls for a building from a layout string.
/// The walls and corners are automatically detected to use the corresponding double-thickness pipe characters.
struct BuildingWallEntity : Entity {
    /// Initialize a new building position entity from a layout string
    /// e.g.
    /// std::vector<std::string> layout = {
    ///     "xxxxx",
    ///     "x   x",
    ///     "x   x",
    ///     "x   x",
    ///     "xx xx"
    /// }
    /// would be a small hut with a door.
    /// \param pos world position of top left corner of building
    /// \param layout vector of strings, each line being a row of the building. Each cross 'x' becomes a wall of the
    /// building
    explicit BuildingWallEntity(const Point &pos, const std::vector<std::string> &layout);

    /// Overrides rendering to display the correct tile for each WallType
    /// \param font the font to render on
    /// \param currentWorldPos the current world position
    void render(Font &font, Point currentWorldPos) override;

    /// Overrides collision detection to take the numerous walls into account
    /// \param pos position to check if colliding
    /// \return whether or not collision occurred
    bool collide(const Point &pos) override;

    /// The different allowed wall types
    enum class WallType : int {
        UL_CORNER, // corner with adjacent walls above and to the left
        UR_CORNER, // corner w/ walls above and to the right
        DL_CORNER, // corner w/ walls below and to the left
        DR_CORNER, // corner w/ walls below and to the right
        VERT,      // vertical wall with walls above and below
        HORIZ,     // horizontal wall with walls to left and right
        CROSS,     // 4 way corner with walls in every direction
        T_UP,      // T junction with walls above, to left, and to right
        T_LEFT,    // T junction with walls above, to left, and down
        T_DOWN,    // T junction with walls to left, down, and to right
        T_RIGHT    // T junction with walls down, to right, and up
    };

  private:
    std::unordered_map<Point, WallType> mWalls;

    /// Take a set of points and populate mWalls with the correct WallTypes
    /// \param points the set of points that represent the building
    void generateWallsFromPoints(const std::unordered_set<Point> &points);
};
