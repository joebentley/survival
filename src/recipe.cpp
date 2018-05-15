#include "recipe.h"

RecipeManager::RecipeManager() {
    recipes.emplace_back(FireRecipe());
}

int FireRecipe::numProduced = 0;
void FireRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto fire = std::make_shared<FireEntity>("fire" + std::to_string(++numProduced));
    fire->setPos(player->pos);
    em.addEntity(fire);
}
