#include "behaviours.h"
#include "world.h"

void PlayerInputBehaviour::handle(const std::string& event) {
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

    if (parent.x < 0) {
        parent.x = SCREEN_WIDTH - 1;
        parent.worldX--;
    } else if (parent.x >= SCREEN_WIDTH) {
        parent.x = 0;
        parent.worldX++;
    }
    if (parent.y < 0) {
        parent.y = SCREEN_HEIGHT - 1;
        parent.worldY--;
    } else if (parent.y >= SCREEN_HEIGHT) {
        parent.y = 0;
        parent.worldY++;
    }
}