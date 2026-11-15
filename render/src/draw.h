#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void draw_highlight(SDL_Surface *s, double ax, double ay, double bx, double by,
                    double radius, SDL_Color color, double alpha);

#endif
