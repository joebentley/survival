
#ifndef POINT_H_
#define POINT_H_

#include <cmath>

struct Point {
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}

    friend Point operator-(Point lhs, const Point& rhs) {
        return Point(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    friend Point operator+(Point lhs, const Point& rhs) {
        return Point(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    double distanceTo(const Point& to) {
        return (*this - to).length();
    }

    double length() {
        return std::sqrt(x*x + y*y);
    }
};

#endif // POINT_H_