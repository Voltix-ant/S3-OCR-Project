#ifndef SOLUTION_IMAGE_H
#define SOLUTION_IMAGE_H

/* This file defines the function that draw the solution on an image :
    a capsul of color for each word.*/

#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "common/src/word_result.h"
#include "analysis/src/analysis.h"

void draw_solution(SDL_Surface *s, const struct analysis *a,
                   const struct found_word *words, size_t nb_words);

#endif
