#pragma once

#include "../Behaviour/InteractableBehaviour.h"
#include "Entity.h"

struct FireEntity : Entity {
    struct RekindleBehaviour : InteractableBehaviour {
        explicit RekindleBehaviour(Entity &parent) : InteractableBehaviour(parent) {}

        bool handleInput(SDL_KeyboardEvent &e) override;
        void render(Font &font) override;

      private:
        bool choosingItemToUse{false};
        int choosingItemIndex{0};
    };

    explicit FireEntity(std::string ID = "");

    void render(Font &font, Point currentWorldPos) override;
    void tick() override;

    float fireLevel{1};
};