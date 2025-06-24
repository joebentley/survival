#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL.h>

#include <string>
#include <vector>
#include "Point.h"
#include "FontColor.h"

/// Handles an SDL texture, and releases it when it goes out of scope
class Texture {
public:
    /// Construct a new texture object that can be allocated using `loadFromFile`
    /// \param renderer the SDL_Renderer instance that the texture should render onto
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
    /// \param dst a rectangle representing where and with what size to render the texture on the renderer
    void render(SDL_Rect *src, SDL_Rect *dst);

    /// Return the underlying texture
    SDL_Texture *getTexture() const;
    /// Return the pixel format of the texture
    Uint32 getFormat() const;
    /// Return the access rights? of the texture
    int getAccess() const;
    /// Return the width in pixels
    int getWidth() const;
    /// Return the height in pixels
    int getHeight() const;
protected:
    SDL_Renderer *mRenderer {nullptr};
    SDL_Texture *mTexture {nullptr};
    Uint32 mFormat {0};
    int mAccess {0};
    int mWidth {0};
    int mHeight {0};
};

struct Color;
/// Represents a light point to be rendered with a given point, light radius, and color
struct LightMapPoint {
    LightMapPoint(Point p, int radius, Color color) : mPoint(p), mRadius(radius), mColor(color) {}
    LightMapPoint(Point p, int radius) : mPoint(p), mRadius(radius), mColor(FontColor::getColor("white")) {}
    LightMapPoint() : mPoint(Point(0, 0)), mRadius(0), mColor(FontColor::getColor("white")) {}

    Point mPoint;
    int mRadius;
    struct Color mColor;
};

/// Represents a texture used to render the light point
class LightMapTexture : Texture {
public:
    /// Loads and allocates the light map texture for drawing onto the renderer
    explicit LightMapTexture(SDL_Renderer *renderer);

    // Delete copy constructor and copy assignment
    explicit LightMapTexture(const LightMapTexture &) = delete;
    LightMapTexture &operator=(const LightMapTexture &) = delete;

    ~LightMapTexture() override;

    /// Renderer all the points given to the screen with backgroundAlpha representing the overall day night cycle
    /// \param points the light points on the screen
    /// \param backgroundAlpha the alpha value of the overall background fog
    void render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha);
private:
    SDL_Texture *mNightFadeTexture {nullptr};
};

#endif // TEXTURE_H_