#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "point.h"


struct Texture {
    SDL_Texture *texture {nullptr};

    Uint32 format;
    int access;
    int width;
    int height;

    ~Texture();

    void render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst);

    static int LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath);
};

struct LightMapPoint {
    LightMapPoint(Point p, int radius) : p(p), radius(radius) {}
    LightMapPoint() : p(Point(0, 0)), radius(0) {}

    Point p;
    int radius;
};

struct LightMapTexture {
    explicit LightMapTexture(SDL_Renderer *renderer) : renderer(renderer) {}
    ~LightMapTexture();

    int load();
    void init();
    void render(std::vector<LightMapPoint> points, Uint8 backgroundAlpha);
private:
    SDL_Renderer *renderer;
    Texture lightTexture;
    SDL_Texture *nightFadeTexture {nullptr};
};

#endif // TEXTURE_H_