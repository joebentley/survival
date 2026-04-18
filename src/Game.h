#ifndef SURVIVAL_GAME_H
#define SURVIVAL_GAME_H

#include "Entities.h"
#include "Font.h"
#include "SDLManager.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

const int MAX_FRAME_RATE = 30;

class Game {
  public:
    Game();
    void run();

  private:
    void loop();

    std::unique_ptr<Texture> makeFontTexture();

    SDLManager mSDLManager;
    LightMapTexture m_lightMapTexture;
    std::unique_ptr<Texture> mFontTexture{nullptr};
    Font m_font;
    World m_world;
    PlayerEntity *m_player;
};

#endif // SURVIVAL_GAME_H
