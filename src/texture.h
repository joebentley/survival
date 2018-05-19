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
    SDL_Texture *texture {nullptr};

    Uint32 format {0};
    int access {0};
    int width {0};
    int height {0};

    ~Texture();

    void render(SDL_Renderer *renderer, SDL_Rect *src, SDL_Rect *dst);

    static int LoadFromFile(Texture &texture, SDL_Renderer* renderer, const std::string& filepath);
};

struct Color;
struct LightMapPoint {
    LightMapPoint(Point p, int radius, Color color) : p(p), radius(radius), color(color) {}
    LightMapPoint(Point p, int radius) : p(p), radius(radius), color(getColor("white")) {}
    LightMapPoint() : p(Point(0, 0)), radius(0), color(getColor("white")) {}

    Point p;
    int radius;
    struct Color color;
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