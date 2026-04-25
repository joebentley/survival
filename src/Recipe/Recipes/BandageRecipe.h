#pragma once

#include "../Recipe.h"

struct BandageRecipe : Recipe {
    static int numProduced;

    BandageRecipe();

    void produce() override;
};
