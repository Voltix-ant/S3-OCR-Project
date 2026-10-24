#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


Uint32 get_pixel(SDL_Surface *surface, int x, int y);
int set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
int grayscale(SDL_Surface *surface);
int adaptive_threshold(SDL_Surface *surface);

#endif
