#ifndef TEXTURE_H_
#define TEXTURE_H_

#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <string>
#include <vector>
#include "point.h"
#include "font.h"

struct Texture {
    SDL_Texture *mTexture {nullptr};

    Uint32 mFormat {0};
    int mAccess {0};
    int mWidth {0};
    int mHeight {0};

    ~Texture();

    void render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst);

    static int LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath);
};

struct Color;
struct LightMapPoint {
    LightMapPoint(Point p, int radius, Color color) : mPoint(p), mRadius(radius), mColor(color) {}
    LightMapPoint(Point p, int radius) : mPoint(p), mRadius(radius), mColor(getColor("white")) {}
    LightMapPoint() : mPoint(Point(0, 0)), mRadius(0), mColor(getColor("white")) {}

    Point mPoint;
    int mRadius;
    struct Color mColor;
};

struct LightMapTexture {
    explicit LightMapTexture(SDL_Renderer *renderer) : mRenderer(renderer) {}
    ~LightMapTexture();

    int load();
    void init();
    void render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha);
private:
    SDL_Renderer *mRenderer;
    Texture mLightTexture;
    SDL_Texture *mNightFadeTexture {nullptr};
};

#endif // TEXTURE_H_