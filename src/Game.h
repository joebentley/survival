#ifndef SURVIVAL_GAME_H
#define SURVIVAL_GAME_H

#include <ctime>
#include <cstdlib>
#include <iostream>
#include "SDLManager.h"

const int MAX_FRAME_RATE = 30;

class Game {
public:
    int init();
    int run();

private:
    int loop();

    SDLManager *mSDLManager {nullptr};
    std::unique_ptr<Texture> mFontTexture {nullptr};
};

#endif //SURVIVAL_GAME_H
