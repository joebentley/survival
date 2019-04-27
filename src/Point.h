
#ifndef POINT_H_
#define POINT_H_

#include <cmath>
#include <string>

struct Point {
    int mX;
    int mY;

    Point() : mX(0), mY(0) {}
    Point(int x, int y) : mX(x), mY(y) {}

    bool operator==(const Point &rhs) const {
        return mX == rhs.mX &&
               mY == rhs.mY;
    }

    bool operator!=(const Point &rhs) const {
        return !(rhs == *this);
    }

    Point& operator+=(const Point& rhs) {
        this->mX = this->mX + rhs.mX;
        this->mY = this->mY + rhs.mY;
        return *this;
    }

    Point& operator-=(const Point& rhs) {
        this->mX = this->mX - rhs.mX;
        this->mY = this->mY - rhs.mY;
        return *this;
    }

    Point& operator*=(int rhs) {
        this->mX *= rhs;
        this->mY *= rhs;
        return *this;
    }

    Point& operator/=(int rhs) {
        this->mX /= rhs;
        this->mY /= rhs;
        return *this;
    }

    // element-wise multiplication
    Point& operator*=(const Point& rhs) {
        this->mX *= rhs.mX;
        this->mY *= rhs.mY;
        return *this;
    }

    double distanceTo(const Point& to);

    double length() {
        return std::sqrt(mX*mX + mY*mY);
    }

	std::string to_string() const {
		return "(" + std::to_string(mX) + ", " + std::to_string(mY) + ")";
	}

	int manhattanDistanceTo(const Point &other) const {
        return std::abs(other.mX - this->mX) + std::abs(other.mY - this->mY);
    }

    Point& abs() {
        this->mX = std::abs(mX);
        this->mY = std::abs(mY);
        return *this;
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
			return (p.mY << 16) ^ p.mX;
//            return std::hash<std::string>{}(p.to_string());
		}
	};
}

#endif // POINT_H_