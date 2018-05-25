#include "Game.h"

int main(int argc, char* argv[])
{
    Game game;
    if (game.init() != 0)
        return -1;
    else
        game.run();
    return 0;
}