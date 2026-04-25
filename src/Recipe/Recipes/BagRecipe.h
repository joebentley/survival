#pragma once

#include "../Recipe.h"

struct BagRecipe : Recipe {
    BagRecipe();

    void produce() override;
};