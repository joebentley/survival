#include "TorchRecipe.h"

#include "../../Entity/EntityManager.h"
#include "../../Entity/Items/TorchEntity.h"

TorchRecipe::TorchRecipe() : Recipe("Torch") {
    mIngredients.emplace_back(Ingredient{1, "wood"});
    mIngredients.emplace_back(Ingredient{1, "grass"});
}

int TorchRecipe::numProduced = 0;
void TorchRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto torch = std::make_unique<TorchEntity>("torch" + std::to_string(++numProduced));
    auto ID = torch->mID;
    EntityManager::getInstance().addEntity(std::move(torch));
    player->addToInventory(ID);
}
