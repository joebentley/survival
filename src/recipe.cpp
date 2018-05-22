#include "recipe.h"

RecipeManager::RecipeManager() {
    mRecipes.emplace_back(std::make_shared<FireRecipe>());
    mRecipes.emplace_back(std::make_shared<BandageRecipe>());
    mRecipes.emplace_back(std::make_shared<TorchRecipe>());
    mRecipes.emplace_back(std::make_shared<BagRecipe>());
}

int FireRecipe::numProduced = 0;
void FireRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto fire = std::make_shared<FireEntity>("fire" + std::to_string(++numProduced));
    fire->setPos(mPointIfNotGoingIntoInventory);
    em.addEntity(fire);
}

int BandageRecipe::numProduced = 0;
void BandageRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto bandage = std::make_shared<BandageEntity>("bandage" + std::to_string(++numProduced));
    player->addToInventory(bandage);
}

int TorchRecipe::numProduced = 0;
void TorchRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto torch = std::make_shared<TorchEntity>("torch" + std::to_string(++numProduced));
    player->addToInventory(torch);
}

void BagRecipe::produce() {
    auto &em = EntityManager::getInstance();
    auto player = em.getEntityByID("Player");
    auto torch = std::make_shared<BagEntity>();
    player->addToInventory(torch);
}
