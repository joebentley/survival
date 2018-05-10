#include "behaviours.h"
#include "entities.h"
#include "world.h"
#include <cstdlib>
#include <iostream>

void WanderBehaviour::tick() {
    switch (rand() % 20) {
        case 0:
            parent.pos.x++;
            break;
        case 1:
            parent.pos.x--;
            break;
        case 2:
            parent.pos.y++;
            break;
        case 3:
            parent.pos.y--;
            break;
        case 4:
            parent.pos.x++;
            parent.pos.y++;
            break;
        case 5:
            parent.pos.x--;
            parent.pos.y++;
            break;
        case 6:
            parent.pos.x++;
            parent.pos.y--;
            break;
        case 7:
            parent.pos.x--;
            parent.pos.y--;
            break;
    }
}

void AttachmentBehaviour::tick() {
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    if (!attached) {
        if (r < attachment) {
            PlayerEntity& p = static_cast<PlayerEntity&>(*parent.manager.getByID("Player"));
            if (parent.pos.distanceTo(p.pos) < 10) {
                std::cout << parent.ID << " has attached itself to player!" << std::endl;
                attached = true;
            }
        }
        return;
    }

    if (r < clinginess) {
        PlayerEntity& p = static_cast<PlayerEntity&>(*parent.manager.getByID("Player"));

        if (parent.pos.x < p.pos.x)
            parent.pos.x++;
        else if (parent.pos.x > p.pos.x)
            parent.pos.x--;
        if (parent.pos.y < p.pos.y)
            parent.pos.y++;
        else if (parent.pos.y > p.pos.y)
            parent.pos.y--;
    }

    if (r < unattachment) {
        std::cout << parent.ID << " has detached itself from player!" << std::endl;
        attached = false;
        return;
    }
}

void PlayerInputBehaviour::handle(const std::string& event) {
    parent.manager.tick(); // Only tick on player movement

    if (event == "input up")
        parent.pos.y--;
    else if (event == "input down")
        parent.pos.y++;        
    else if (event == "input left")
        parent.pos.x--;
    else if (event == "input right")
        parent.pos.x++;
    else if (event == "input upleft") {
        parent.pos.y--;
        parent.pos.x--;
    } else if (event == "input upright") {
        parent.pos.y--;
        parent.pos.x++;        
    } else if (event == "input downleft") {
        parent.pos.y++;
        parent.pos.x--;
    } else if (event == "input downright") {
        parent.pos.y++;
        parent.pos.x++;
    }
}