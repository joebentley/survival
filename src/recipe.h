#ifndef RECIPE_H_
#define RECIPE_H_

#include <vector>
#include <string>

struct Recipe {
    struct Ingredient {
        int quantity;
        std::string entityType;
    };

    std::vector<Ingredient> ingredients;

    virtual void produce();
};


#endif //RECIPE_H_
