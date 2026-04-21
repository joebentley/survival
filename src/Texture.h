#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL3/SDL.h>

#include "FontColor.h"
#include "Point.h"
#include <string>
#include <vector>

/// Handles an SDL texture, and releases it when it goes out of scope
class Texture {
  public:
    /// Construct a new texture object that can be allocated using `loadFromFile`
    /// \param renderer the SDL_Renderer instance that the texture should render
    /// onto
    explicit Texture(SDL_Renderer *renderer) : mRenderer(renderer) {}

    // Delete copy constructor and copy assignment
    explicit Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    /// Release the underlying texture if allocated
    virtual ~Texture();

    /// Load the texture given by the filepath using SDL_image
    /// \param filepath relative filepath to the texture
    /// \return 0 if loaded successfully, otherwise -1
    int loadFromFile(const std::string &filepath);

    /// Render the texture onto the SDL_Renderer given by mRenderer
    /// \param src a rectangle representing the section of the texture to render
    /// \param dst a rectangle representing where and with what size to render the
    /// texture on the renderer
    void render(SDL_FRect *src, SDL_FRect *dst);

    /// Return the underlying texture
    SDL_Texture *getTexture() const;
    /// Return the pixel format of the texture
    SDL_PixelFormat getFormat() const;
    /// Return the width in pixels
    int getWidth() const;
    /// Return the height in pixels
    int getHeight() const;

  protected:
    SDL_Renderer *mRenderer{nullptr};
    SDL_Texture *mTexture{nullptr};
};

#endif // TEXTURE_H_