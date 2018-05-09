#include "behaviours.h"

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
}