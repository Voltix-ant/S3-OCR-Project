#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "../../common/src/matrix.h"


Uint32 get_pixel(SDL_Surface *surface, int x, int y);
int set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
int grayscale(SDL_Surface *surface);
int adaptive_threshold(SDL_Surface *surface);
SDL_Surface *rotate_surface(SDL_Surface *src, double angle_deg);
struct matrix *surface_to_matrix(SDL_Surface *s);

#endif
