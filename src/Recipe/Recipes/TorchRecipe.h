#pragma once

#include "../Recipe.h"

struct TorchRecipe : Recipe {
    static int numProduced;

    TorchRecipe();

    void produce() override;
};
