#ifndef SURVIVAL_GAME_H
#define SURVIVAL_GAME_H

#include "Entities.h"
#include "Font.h"
#include "LightMapTexture.h"
#include "SDLManager.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

const int MAX_FRAME_RATE = 30;

class Game {
  public:
    Game();
    ~Game();

    void iterate();
    bool processEvent(SDL_Event *e);

  private:
    std::unique_ptr<Texture> makeFontTexture();
    PlayerEntity *makePlayer();

    SDLManager mSDLManager;
    LightMapTexture m_lightMapTexture;
    std::unique_ptr<Texture> mFontTexture{nullptr};
    Font m_font;
    World m_world;
    PlayerEntity *m_player;
    Screens m_screens;

    SDL_Texture *m_renderTexture;

    StatusUIEntity *m_pStatusUI;

    std::vector<std::string> m_initialMessageLines;

    std::deque<float> m_frameTimes;
    float m_totalTime = 0;
    float m_fps = 60;

    bool m_initialMessage = true;
};

#endif // SURVIVAL_GAME_H
