#pragma once

#include "../../Behaviours.h"
#include "../../Properties.h"
#include "../Entity.h"

struct GlowbugEntity : Entity {
    explicit GlowbugEntity(std::string ID = "") : Entity(std::move(ID), "Glowbug", "$[green]`", 10.0f, 10.0f, 0.05f) {
        addBehaviour(std::make_unique<WanderBehaviour>(*this));
        addProperty(std::make_unique<LightEmittingProperty>(this, 3, Color::getColor("green")));
    }

    void render(Font &font, Point currentWorldPos) override;
};
