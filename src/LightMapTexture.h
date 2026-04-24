#pragma once

#include "Texture.h"
#include <vector>

struct LightMapPoint;
/// Represents a texture used to render the light point
class LightMapTexture : Texture {
  public:
    /// Loads and allocates the light map texture for drawing onto the renderer
    explicit LightMapTexture(SDL_Renderer *renderer);

    // Delete copy constructor and copy assignment
    explicit LightMapTexture(const LightMapTexture &) = delete;
    LightMapTexture &operator=(const LightMapTexture &) = delete;

    ~LightMapTexture() override;

    /// Renderer all the points given to the screen with backgroundAlpha
    /// representing the overall day night cycle
    /// \param points the light points on the screen
    /// \param backgroundAlpha the alpha value of the overall background fog
    void render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha);

  private:
    SDL_Texture *mNightFadeTexture{nullptr};
};