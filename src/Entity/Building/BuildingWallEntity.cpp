#include "BuildingWallEntity.h"
#include "../EntityManager.h"
#include "DoorEntity.h"

BuildingWallEntity::BuildingWallEntity(const Point &pos, const std::vector<std::string> &layout)
    : Entity("", "Wall", "") {
    this->setPos(pos);

    // generate the walls from the given layout string
    int x = 0;
    int y = 0;

    std::unordered_set<Point> walls;

    for (const std::string &line : layout) {
        for (char character : line) {
            if (character == '+') {
                walls.insert(Point(x, y));
            } else if (character == 'd') {
                auto door = std::make_unique<DoorEntity>(pos + Point(x, y));
                EntityManager::getInstance().addEntity(std::move(door));
            }

            x++;
        }
        x = 0;
        y++;
    }

    // generate the correct wall tile types
    generateWallsFromPoints(walls);

    // render on top
    mRenderingLayer = -1;

    // should not be able to attack the wall
    mCanBeAttacked = false;
}

/// To neatly check if a set of Points has a given Point. There's probably a better way but this works fine
struct SetHas {
    explicit SetHas(std::unordered_set<Point> points) : points(std::move(points)) {}

    bool has(const Point &pos) { return points.find(pos) != points.cend(); }

    std::unordered_set<Point> points;
};

void BuildingWallEntity::generateWallsFromPoints(const std::unordered_set<Point> &points) {
    auto h = SetHas(points);

    // Work out the neighbour points for each point, and choose the correct WallType correspondingly
    for (const Point &p : points) {
        if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)) && h.has(p + Point(1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::CROSS});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::T_LEFT});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::T_UP});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::T_RIGHT});
        else if (h.has(p + Point(1, 0)) && h.has(p + Point(-1, 0)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::T_DOWN});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(-1, 0)))
            mWalls.insert({p, WallType::UL_CORNER});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::UR_CORNER});
        else if (h.has(p + Point(0, 1)) && h.has(p + Point(-1, 0)))
            mWalls.insert({p, WallType::DL_CORNER});
        else if (h.has(p + Point(0, 1)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::DR_CORNER});
        else if (h.has(p + Point(0, -1)) && h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::VERT});
        else if (h.has(p + Point(-1, 0)) && h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::HORIZ});
        else if (h.has(p + Point(0, -1)) || h.has(p + Point(0, 1)))
            mWalls.insert({p, WallType::VERT});
        else if (h.has(p + Point(-1, 0)) || h.has(p + Point(1, 0)))
            mWalls.insert({p, WallType::HORIZ});
        else
            mWalls.insert({p, WallType::CROSS}); // this should just be lonely wall tiles
    }
}

void BuildingWallEntity::render(Font &font, Point currentWorldPos) {
    // Only draw if the entity is on the current world screen
    if (isOnScreen(currentWorldPos)) {
        // draw each wall tile
        for (const auto &pair : mWalls) {
            Point wallPos = pair.first;
            Point screenPos = World::worldToScreen(mPos + wallPos);
            WallType wallType = pair.second;

            std::string c; // the character we will draw

            // choose the correct fontstring character for each wall type. Here we choose the double-thickness pipe
            // walls
            switch (wallType) {
            case WallType::CROSS:
                c = "p28";
                break;
            case WallType::T_LEFT:
                c = "p7";
                break;
            case WallType::T_RIGHT:
                c = "p26";
                break;
            case WallType::T_UP:
                c = "p24";
                break;
            case WallType::T_DOWN:
                c = "p25";
                break;
            case WallType::UL_CORNER:
                c = "p10";
                break;
            case WallType::UR_CORNER:
                c = "p22";
                break;
            case WallType::DL_CORNER:
                c = "p9";
                break;
            case WallType::DR_CORNER:
                c = "p23";
                break;
            case WallType::VERT:
                c = "p8";
                break;
            case WallType::HORIZ:
                c = "p27";
                break;
            }

            font.draw(c, screenPos, Color::getColor("white"), Color::getColor("black"));
        }
    }
}

bool BuildingWallEntity::collide(const Point &pos) {
    // the grid position on our BuildingWallEntity's "blueprints" (i.e. mWalls)
    Point wallPos = pos - mPos;
    // check if this point is in mWalls
    return mWalls.find(wallPos) != mWalls.cend();
}