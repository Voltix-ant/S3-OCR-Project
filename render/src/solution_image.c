#include "solution_image.h"
#include "analysis/src/analysis.h"
#include "draw.h"
#include <err.h>

static void compute_avg_ctr(const struct analysis *a, 
        double **row_y, double **col_x, double *grid_radius) {
    size_t n_row = analysis_grid_rows(a);
    size_t n_col = analysis_grid_cols(a);
    if (n_row == 0 || n_col == 0) return;

    *row_y = calloc(n_row,sizeof(double));
    if (!row_y) 
        errx(1, "compute_avg_ctr : couldn't allocate memory");
    for (size_t y = 0; y < n_row; y++) {
        double sum = 0;
        for (size_t x = 0; x < n_col; x++) {
            struct box b = analysis_grid_box(a,y,x);
            sum += (b.y2 + b.y1) / 2;
        }
        (*row_y)[y] = sum / n_col;
    }
    *col_x = calloc(n_col,sizeof(double));
    if (!col_x) 
        errx(1, "compute_avg_ctr : couldn't allocate memory");
    for (size_t x = 0; x < n_col; x++) {
        double sum = 0;
        for (size_t y = 0; y < n_row; y++) {
            struct box b = analysis_grid_box(a,y,x);
            sum += (b.x2 + b.x1) / 2;
        }
        (*col_x)[x] = sum / n_row;
    }

    double row_gap = (n_row > 1)
        ? ((*row_y)[n_row - 1] - (*row_y)[0]) / (n_row - 1) : 0.0;
    double col_gap = (n_col > 1)
        ? ((*col_x)[n_col - 1] - (*col_x)[0]) / (n_col - 1) : 0.0;

    // the capsule must not overflow on the neighbour rows or columns :
    // take the smallest of the two gaps
    double cell = row_gap;
    if (cell <= 0.0 || (col_gap > 0.0 && col_gap < cell))
        cell = col_gap;

    if (cell > 0.0) {
        *grid_radius = 0.4 * cell;
    } else {
        // grid of one row and one column : fall back on the letter size
        struct box b = analysis_grid_box(a, 0, 0);
        *grid_radius = 0.6 * (b.y2 - b.y1 + 1);
    }
}

static double word_center_y(const struct analysis *a, size_t w) {
    /* Mean vertical center of the letters of the word w */
    size_t len = analysis_word_len(a, w);
    if (len == 0)
        return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < len; i++) {
        struct box b = analysis_word_box(a, w, i);
        sum += (b.y1 + b.y2) / 2.0;
    }
    return sum / len;
}

static void word_capsule(const struct analysis *a, size_t w,
                         double *ax, double *bx, double *y, double *radius) {
    /* Computes the capsule of the word w in the word list : the two end
    points (ax, y) and (bx, y), and the radius. The radius follows the size
    of the letters, but stays small enough not to overlap the neighbours. */
    size_t len = analysis_word_len(a, w);
    if (len == 0) {
        *ax = *bx = *y = *radius = 0.0;   // radius 0 : nothing is drawn
        return;
    }

    struct box l1 = analysis_word_box(a, w, 0);
    struct box l2 = analysis_word_box(a, w, len - 1);
    *ax = (l1.x1 + l1.x2) / 2.0;
    *bx = (l2.x1 + l2.x2) / 2.0;

    // vertical center of the word
    *y = word_center_y(a, w);

    // height of the word : from the highest top to the lowest bottom
    int top = l1.y1;
    int bottom = l1.y2;
    for (size_t i = 1; i < len; i++) {
        struct box b = analysis_word_box(a, w, i);
        if (b.y1 < top)
            top = b.y1;
        if (b.y2 > bottom)
            bottom = b.y2;
    }
    double h = bottom - top + 1;

    // wanted radius
    *radius = 0.6 * h;

    // limit given by the neighbour words (previous and next)
    size_t nb_words = analysis_nb_words(a);
    double gap = -1.0;                    // -1 : no neighbour found
    for (int k = -1; k <= 1; k += 2) {
        if (k == -1 && w == 0)
            continue;                     // no previous word
        size_t v = w + k;                 // w - 1 or w + 1
        if (v >= nb_words)
            continue;                     // no next word
        double d = fabs(word_center_y(a, v) - *y);
        // ignore a word on the same line or far away (other column)
        if (d > 0.0 && d < 3.0 * h && (gap < 0.0 || d < gap))
            gap = d;
    }
    if (gap > 0.0 && 0.45 * gap < *radius)
        *radius = 0.45 * gap;
}

void draw_solution(SDL_Surface *s, const struct analysis *a,
                   const struct found_word *words, size_t nb_words) {
    double *row_y = NULL;
    double *col_x = NULL;
    double grid_radius = 0.0;

    double alpha = 0.4; // Adjust transparency

    compute_avg_ctr(a,&row_y,&col_x,&grid_radius);
    for (size_t w = 0; w < nb_words; w++) {
        double ax,bx,y,wl_radius;
        word_capsule(a, w, &ax, &bx, &y, &wl_radius);
        if (words[w].found) {
            // capsul in the wordlist :
            draw_highlight(s, ax, y, bx, y, wl_radius, word_color(w), alpha);

            // capsul in grid :
            draw_highlight(s, col_x[words[w].c1], row_y[words[w].r1],
                              col_x[words[w].c2], row_y[words[w].r2],
                    grid_radius, word_color(w), alpha);
        } else {
            draw_frame(s, ax, y, bx, y, wl_radius);

        }
    }

    free(row_y);
    free(col_x);
}

