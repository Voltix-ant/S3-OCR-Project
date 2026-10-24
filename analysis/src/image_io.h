#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include "common/src/matrix.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <err.h>

// Functions

SDL_Surface *load_image(const char *path);
Uint32 get_px(SDL_Surface *s, int x, int y);
struct matrix *img_to_matrix(SDL_Surface *s);
void binarize_matrix(struct matrix *m, size_t block_size, char C);
SDL_Surface *matrix_to_img(struct matrix *m);

#endif
