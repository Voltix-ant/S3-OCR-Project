#include "draw.h"

static void blend_pixel(SDL_Surface *s, int x, int y, SDL_Color color, 
        double alpha) {
    /* This function modifie the pixel (x,y) to apply a color above :
    new = old * (1 - alpha) + color * alpha*/
    if (x < 0 || y < 0 || x >= s->w || y >= s->h) return;
    if (alpha <= 0.0)
        return;
    if (alpha > 1.0)
        alpha = 1.0;
    Uint32 *p = (Uint32 *)((Uint8 *)s->pixels + y * s->pitch) + x;

    Uint8 r, g, b, a;
    SDL_GetRGBA(*p, s->format, &r, &g, &b, &a);

    r = (Uint8)(r * (1.0 - alpha) + color.r * alpha + 0.5);
    g = (Uint8)(g * (1.0 - alpha) + color.g * alpha + 0.5);
    b = (Uint8)(b * (1.0 - alpha) + color.b * alpha + 0.5);

    *p = SDL_MapRGBA(s->format, r, g, b, a);   // alpha channel unchanged
}

SDL_Color word_color(size_t i) {
    /* Color of the word of index i. Loops over the palette if there are
    more words than colors. */
    return WORD_COLORS[i % NB_WORD_COLORS];
}

void draw_highlight(SDL_Surface *s, double ax, double ay, double bx, double by,
                    double radius, SDL_Color color, double alpha) {
    /* This function draw a pill shape from points a to b of certain radius.*/

    if (!s || s->format->BytesPerPixel != 4 || radius <= 0) return;

    // Coord of the rectangle
    int x_min = (int)floor(fmin(ax, bx) - radius);
    int x_max = (int)ceil (fmax(ax, bx) + radius);
    int y_min = (int)floor(fmin(ay, by) - radius);
    int y_max = (int)ceil (fmax(ay, by) + radius);
    if (x_min < 0) x_min = 0;
    if (y_min < 0) y_min = 0;
    if (x_max >= s->w) x_max = s->w - 1;
    if (y_max >= s->h) y_max = s->h - 1;
    double dx = bx - ax;
    double dy = by - ay;
    double len2 = dx * dx + dy * dy;   // squared length of the segment
    double r2 = radius * radius;

    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);

    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            // Position on the segement
            double t = 0.0;
            if (len2 > 0.0)
                t = ((x - ax) * dx + (y - ay) * dy) / len2;
            if (t < 0.0) t = 0.0;
            if (t > 1.0) t = 1.0;

            // distance from t :
            double qx = ax + t * dx;
            double qy = ay + t * dy;
            double d2 = (x - qx) * (x - qx) + (y - qy) * (y - qy);

            // draw pixel
            if (d2 <= r2) {
                blend_pixel(s, x, y, color, alpha);
            }
        }
    }

    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

void draw_frame(SDL_Surface *s, double ax, double ay, double bx, double by,
                double radius) {
    /* This function draws the outline of the rectangle that contains the
    pill shape from a to b of the given radius (same parameters as
    draw_highlight). Meant for horizontal words : the rectangle is not
    rotated. The thickness of the outline is deduced from the radius. */

    if (!s || s->format->BytesPerPixel != 4 || radius <= 0) return;

    // Outer limits of the frame (not clamped yet, to know where the edges are)
    int left   = (int)floor(fmin(ax, bx) - radius);
    int right  = (int)ceil (fmax(ax, bx) + radius);
    int top    = (int)floor(fmin(ay, by) - radius);
    int bottom = (int)ceil (fmax(ay, by) + radius);

    int thick = (int)(0.15 * radius + 0.5);
    if (thick < 2) thick = 2;

    // Pixels actually visited : the frame clamped to the surface
    int x_min = left   < 0 ? 0 : left;
    int y_min = top    < 0 ? 0 : top;
    int x_max = right  >= s->w ? s->w - 1 : right;
    int y_max = bottom >= s->h ? s->h - 1 : bottom;

    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);

    SDL_Color c = {255,0,0,255};
    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            // a pixel belongs to the outline if it is close to one edge
            int on_edge = x < left + thick || x > right - thick
                       || y < top + thick  || y > bottom - thick;
            if (on_edge)
                blend_pixel(s, x, y, c, 1.0);
        }
    }

    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}
