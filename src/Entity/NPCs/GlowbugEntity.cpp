#include "GlowbugEntity.h"
#include "../../Behaviour/AI/WanderBehaviour.h"
#include "../../Property/Properties/LightEmittingProperty.h"

GlowbugEntity::GlowbugEntity(std::string ID) : Entity(std::move(ID), "Glowbug", "$[green]`", 10.0f, 10.0f, 0.05f) {
    addBehaviour(std::make_unique<WanderBehaviour>(*this));
    addProperty(std::make_unique<LightEmittingProperty>(this, 3, Color::getColor("green")));
}

void GlowbugEntity::render(Font &font, Point currentWorldPos) {
    static int timer = 0;

    if (timer++ > (rand() % 20) + 20) {
        switch (rand() % 3) {
        case 0:
            mGraphic = "$[green]`";
            break;
        case 1:
            mGraphic = "$[green]'";
            break;
        case 2:
            mGraphic = "";
            break;
        default:
            break;
        }

        timer = 0;
    }

    Entity::render(font, currentWorldPos);
}