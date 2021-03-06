//
// Created by Joe Bentley on 25/05/2018.
//

#include "FontColor.h"

/// Map of color strings to rgba colors
std::unordered_map<std::string, Color> mColorMap = {
        {"white", Color(0xFF, 0xFF, 0xFF)},
        {"yellow", Color(0xFF, 0xFF, 0)},
        {"red", Color(0xFF, 0, 0)},
        {"green", Color(0, 0xFF, 0)},
        {"grassgreen", Color(0x60, 0x80, 0x38) * 1.5},
        {"grasshay", Color(218, 197, 134)},
        {"blue", Color(0, 0, 0xFF)},
        {"cyan", Color(0, 0xCC, 0xCC)},
        {"grey", Color(0x88, 0x88, 0x88)},
        {"black", Color(0, 0, 0)},
        {"brown", Color(150, 75, 0)},
        {"purple", Color(180, 0, 180)},
        {"orange", Color(255, 128, 0)},
        {"transparent", Color(0, 0, 0, 0)}
};

Color FontColor::getColor(const std::string& colorStr) {
    return mColorMap[colorStr];
}

std::unordered_map<std::string, Color> &FontColor::getColorMap() {
    return mColorMap;
}
