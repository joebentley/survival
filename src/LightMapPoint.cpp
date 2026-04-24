#include "LightMapPoint.h"

LightMapPoint::LightMapPoint(Point p, int radius, Color color) : mPoint(p), mRadius(radius), mColor(color) {}
LightMapPoint::LightMapPoint(Point p, int radius) : mPoint(p), mRadius(radius), mColor(Color::getColor("white")) {}
LightMapPoint::LightMapPoint() : mPoint(Point(0, 0)), mRadius(0), mColor(Color::getColor("white")) {}
