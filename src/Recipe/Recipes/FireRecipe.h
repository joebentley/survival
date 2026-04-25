#pragma once

#include "../Recipe.h"

struct FireRecipe : Recipe {
    static int numProduced;

    FireRecipe();

    void produce() override;
};
