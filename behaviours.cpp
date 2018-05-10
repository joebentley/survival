#include "behaviours.h"
#include "world.h"
#include <cstdlib>

void WanderBehaviour::tick() {
    switch (rand() % 20) {
        case 0:
            parent.x++;
            break;
        case 1:
            parent.x--;
            break;
        case 2:
            parent.y++;
            break;
        case 3:
            parent.y--;
            break;
        case 4:
            parent.x++;
            parent.y++;
            break;
        case 5:
            parent.x--;
            parent.y++;
            break;
        case 6:
            parent.x++;
            parent.y--;
            break;
        case 7:
            parent.x--;
            parent.y--;
            break;
    }
}

void PlayerInputBehaviour::handle(const std::string& event) {
    parent.entityManager.tick(); // Only tick on player movement

    if (event == "input up")
        parent.y--;
    else if (event == "input down")
        parent.y++;        
    else if (event == "input left")
        parent.x--;
    else if (event == "input right")
        parent.x++;
    else if (event == "input upleft") {
        parent.y--;
        parent.x--;
    } else if (event == "input upright") {
        parent.y--;
        parent.x++;        
    } else if (event == "input downleft") {
        parent.y++;
        parent.x--;
    } else if (event == "input downright") {
        parent.y++;
        parent.x++;
    }
}