
#ifndef POINT_H_
#define POINT_H_

#include <cmath>
#include <string>

struct Point {
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}

    bool operator==(const Point &rhs) const {
        return x == rhs.x &&
               y == rhs.y;
    }

    bool operator!=(const Point &rhs) const {
        return !(rhs == *this);
    }

    Point& operator+=(const Point& rhs) {
        this->x = this->x + rhs.x;
        this->y = this->y + rhs.y;
        return *this;
    }

    Point& operator-=(const Point& rhs) {
        this->x = this->x - rhs.x;
        this->y = this->y - rhs.y;
        return *this;
    }

    Point& operator*=(int rhs) {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    Point& operator/=(int rhs) {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }

    // element-wise multiplication
    Point& operator*=(const Point& rhs) {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }

    double distanceTo(const Point& to);

    double length() {
        return std::sqrt(x*x + y*y);
    }

	std::string to_string() const {
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

inline Point operator+(Point lhs, const Point& rhs) {
    lhs += rhs;
    return lhs;
}

inline Point operator-(Point lhs, const Point& rhs) {
    lhs -= rhs;
    return lhs;
}

inline Point operator*(int lhs, Point rhs) {
    rhs *= lhs;
    return rhs;
}

inline Point operator*(Point lhs, int rhs) {
    return rhs * lhs;
}

inline Point operator*(Point lhs, const Point& rhs) {
    lhs *= rhs;
    return lhs;
}

inline Point operator/(Point lhs, int rhs) {
    lhs /= rhs;
    return lhs;
}

inline double Point::distanceTo(const Point& to) {
    return (*this - to).length();
}

namespace std
{
	template <>
	struct hash<Point>
	{
		size_t operator()(const Point& p) const
		{
			// Is this good enough?
			return (p.y << 16) ^ p.x;
		}
	};
}

#endif // POINT_H_