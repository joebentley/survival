#ifndef TEXTURE_H_
#define TEXTURE_H_

#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <string>
#include <vector>
#include "Point.h"
#include "FontColor.h"

class Texture {
public:
    explicit Texture(SDL_Renderer *renderer) : mRenderer(renderer) {}

    virtual ~Texture();

    int loadFromFile(const std::string &filepath);
    void render(SDL_Rect *src, SDL_Rect *dst);

    SDL_Texture *getTexture() const;
    Uint32 getFormat() const;
    int getAccess() const;
    int getWidth() const;
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
struct LightMapPoint {
    LightMapPoint(Point p, int radius, Color color) : mPoint(p), mRadius(radius), mColor(color) {}
    LightMapPoint(Point p, int radius) : mPoint(p), mRadius(radius), mColor(FontColor::getColor("white")) {}
    LightMapPoint() : mPoint(Point(0, 0)), mRadius(0), mColor(FontColor::getColor("white")) {}

    Point mPoint;
    int mRadius;
    struct Color mColor;
};

class LightMapTexture : Texture {
public:
    explicit LightMapTexture(SDL_Renderer *renderer);

    ~LightMapTexture() override;

    void render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha);
private:
    SDL_Texture *mNightFadeTexture {nullptr};
};

#endif // TEXTURE_H_