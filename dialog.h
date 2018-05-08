
#ifndef DIALOG_H_
#define DIALOG_H_

#include <SDL2/SDL.h>
#include <string>
#include "font.h"

void showMessageBox(SDL_Renderer* renderer, Font& font, const std::string& message, int x, int y);

#endif // DIALOG_H_