#pragma once

#include "Color.h"
#include "Point.h"

/// Represents a light point to be rendered with a given point, light radius,
/// and color
struct LightMapPoint {
    LightMapPoint(Point p, int radius, Color color);
    LightMapPoint(Point p, int radius);
    LightMapPoint();

    Point mPoint;
    int mRadius;
    Color mColor;
};