#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <stddef.h>
#include <SDL2/SDL.h>
#include "common/src/matrix.h"
#include "region.h"
#include "components.h"
#include "letter_grid.h"

/* Result of the image analysis. Use the accessor functions below rather
than reading the fields directly, so the internals can change. */
struct analysis {
    struct matrix *mat;              // binarised image (NOT owned)
    int char_h;                      // median letter height, in px
    struct Region *root;             // region tree
    struct Region **leaves;          // leaves of the tree (array of pointers)
    size_t nb_leaves;
    struct Region *grid_region;
    struct letter_grid *grid;        // letters of the grid, row by row
    struct letter_grid **wordlists;  // one per word list region
    size_t nb_wordlists;
};

/* Analyses a binarised image. Returns NULL if no grid is found.
The matrix must stay valid until analysis_destroy() is called. */
struct analysis *analysis_run(struct matrix *mat);
void analysis_destroy(struct analysis *a);

/* ---- Grid : rows and cols start at 0 ---- */
size_t analysis_grid_rows(const struct analysis *a);
size_t analysis_grid_cols(const struct analysis *a);
struct box analysis_grid_box(const struct analysis *a,
                             size_t row, size_t col);
/* 28x28 image of a letter, for the neural network. The caller must free
it with destroy_matrix(). */
struct matrix *analysis_grid_letter(const struct analysis *a,
                                    size_t row, size_t col);

/* ---- Words : all the word lists are seen as one single list ---- */
size_t analysis_nb_words(const struct analysis *a);
size_t analysis_word_len(const struct analysis *a, size_t word);
struct box analysis_word_box(const struct analysis *a,
                             size_t word, size_t idx);
struct matrix *analysis_word_letter(const struct analysis *a,
                                    size_t word, size_t idx);

/* Saves "macro_zones.png" and "letter_grid.png" for debug. s is the
original image. Returns 0 on success, -1 on error. */
int analysis_save_debug(const struct analysis *a, SDL_Surface *s);

#endif
