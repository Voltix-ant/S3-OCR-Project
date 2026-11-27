#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stddef.h>

/* Array of colors each one the most distinct possible from the other ones to
Plug 'artanikin/vim-synthwave84'
   draw the capsuls. */
static const SDL_Color WORD_COLORS[] = {
    {230,  40,  40, 255},
    { 64,  36, 242, 255},
    { 48, 191,  48, 255},
    { 36, 187, 242, 255},
    {230, 103, 196, 255},
    {191, 153,  48, 255},
    { 48,  86, 191, 255},
    { 15, 153, 116, 255},
    {229,  36, 242, 255},
    {230, 129, 103, 255},
    {242,  36, 119, 255},
    { 15,  98, 153, 255},
    {124,  48, 191, 255},
    {116, 153,  15, 255},
    {242, 132,  36, 255},
    {153,  52,  15, 255},
    { 15, 144, 153, 255},
    { 36, 146, 242, 255},
    {153,  15, 135, 255},
    {230, 103, 129, 255},
    { 36,  91, 242, 255},
    { 15, 153,  70, 255},
    {242,  36, 187, 255},
    {196, 103, 230, 255},
    {153,  98,  15, 255},
    {191,  48, 124, 255},
    {160,  36, 242, 255},
    {191,  48,  67, 255},
    {230, 162, 103, 255},
    { 48, 191, 182, 255},
};

#define NB_WORD_COLORS (sizeof(WORD_COLORS) / sizeof(WORD_COLORS[0]))

SDL_Color word_color(size_t i);

void draw_highlight(SDL_Surface *s, double ax, double ay, double bx, double by,
                    double radius, SDL_Color color, double alpha);

#endif
