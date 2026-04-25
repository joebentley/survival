#include "FireRecipe.h"

#include "../../Entity/EntityManager.h"
#include "../../Entity/FireEntity.h"

FireRecipe::FireRecipe() : Recipe("Fire") {
    mIngredients.emplace_back(Ingredient{2, "wood"});
    mIngredients.emplace_back(Ingredient{2, "grass"});
    mGoesIntoInventory = false;
}

int FireRecipe::numProduced = 0;
void FireRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto fire = std::make_unique<FireEntity>("fire" + std::to_string(++numProduced));
    fire->setPos(mPointIfNotGoingIntoInventory);
    em.addEntity(std::move(fire));
}
