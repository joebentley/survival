#pragma once

#include "../Behaviours.h"
#include "../Properties.h"
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

    explicit FireEntity(std::string ID = "") : Entity(std::move(ID), "Fire", "") {
        mIsSolid = true;
        addProperty(std::make_unique<LightEmittingProperty>(this, 6));
        addBehaviour(std::make_unique<RekindleBehaviour>(*this));
    }

    void render(Font &font, Point currentWorldPos) override;
    void tick() override;

    float fireLevel{1};
};