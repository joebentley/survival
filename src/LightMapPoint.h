#pragma once

#include "FontColor.h"
#include "Point.h"

struct Color;
/// Represents a light point to be rendered with a given point, light radius,
/// and color
struct LightMapPoint {
    LightMapPoint(Point p, int radius, Color color) : mPoint(p), mRadius(radius), mColor(color) {}
    LightMapPoint(Point p, int radius) : mPoint(p), mRadius(radius), mColor(FontColor::getColor("white")) {}
    LightMapPoint() : mPoint(Point(0, 0)), mRadius(0), mColor(FontColor::getColor("white")) {}

    Point mPoint;
    int mRadius;
    struct Color mColor;
};