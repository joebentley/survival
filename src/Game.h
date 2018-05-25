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
    void run();

private:
    void loop();

    SDLManager mSDLManager;
    std::unique_ptr<Texture> mFontTexture {nullptr};
};

#endif //SURVIVAL_GAME_H
