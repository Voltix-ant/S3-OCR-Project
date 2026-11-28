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

#include <stdio.h>
void draw_solution(SDL_Surface *s, const struct analysis *a,
                   const struct found_word *words, size_t nb_words) {
    double *row_y = NULL;
    double *col_x = NULL;
    double grid_radius = 0.0;

    double alpha = 0.4; // Adjust transparency

    compute_avg_ctr(a,&row_y,&col_x,&grid_radius);
printf("radius = %.2f, bpp = %d, surface %dx%d\n",
       grid_radius, s->format->BytesPerPixel, s->w, s->h);    for (size_t w = 0; w < nb_words; w++) {
        if (words[w].found) {
            printf("(%.2f,%.2f) -> (%.2f,%.2f)\n",
                    col_x[words[w].c1], row_y[words[w].r1],
                    col_x[words[w].c2], row_y[words[w].r2]);
            draw_highlight(s, col_x[words[w].c1], row_y[words[w].r1],
                              col_x[words[w].c2], row_y[words[w].r2],
                    grid_radius, word_color(w), alpha);
        }
    }

    free(row_y);
    free(col_x);
}
