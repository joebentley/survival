#pragma once

#include "../../Entity.h"

struct TwigEntity : Entity {
    const std::string SHORT_DESC = "A thin, brittle twig";
    const std::string LONG_DESC = "It looks very useful! Who knows where it came from...";

    explicit TwigEntity(std::string ID = "");
};
