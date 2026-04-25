#include "BagRecipe.h"

#include "../../Entity/EntityManager.h"
#include "../../Entity/Items/BagEntity.h"

BagRecipe::BagRecipe() : Recipe("Bag") { mIngredients.emplace_back(Ingredient{5, "grass"}); }

void BagRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto bag = std::make_unique<BagEntity>();
    auto ID = bag->mID;
    EntityManager::getInstance().addEntity(std::move(bag));
    player->addToInventory(ID);
}
