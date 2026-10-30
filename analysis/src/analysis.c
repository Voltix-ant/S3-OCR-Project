#include <stdlib.h>
#include <stdio.h>
#include "analysis.h"
#include "segmentation.h"
#include "classification.h"
#include "word_list.h"
#include "char_image.h"
#include "debug_draw.h"

// ===================== Internal helpers =====================

static size_t row_len(const struct letter_grid *g, size_t r) {
    size_t end = (r + 1 < g->nb_rows) ? g->row_start[r + 1]
                                      : g->letters->count;
    return end - g->row_start[r];
}

/* Finds the word list containing the word number `word` (global index).
Writes the row of the word inside this list in *row. NULL if not found. */
static const struct letter_grid *locate_word(const struct analysis *a,
                                             size_t word, size_t *row) {
    for (size_t i = 0; i < a->nb_wordlists; i++) {
        const struct letter_grid *g = a->wordlists[i];
        if (word < g->nb_rows) {
            *row = word;
            return g;
        }
        word -= g->nb_rows;
    }
    return NULL;
}

// ===================== Main functions =====================

void analysis_destroy(struct analysis *a) {
    if (!a)
        return;
    for (size_t i = 0; i < a->nb_wordlists; i++)
        destroy_letter_grid(a->wordlists[i]);
    free(a->wordlists);
    destroy_letter_grid(a->grid);
    free(a->leaves);
    if (a->root)
        region_destroy(a->root);
    free(a);
}

struct analysis *analysis_run(struct matrix *mat) {
    struct analysis *a = calloc(1, sizeof(struct analysis));
    if (!a)
        return NULL;
    a->mat = mat;

    // ---- Macro zones detection ----
    // gap threshold, in number of letter heights (provisional : works on
    // the test images from 1.0 to 1.9, middle = 1.5)
    const double gap_factor = 1.5;

    a->char_h = median_char_height(mat);
    if (a->char_h == 0) {            // nothing that looks like a letter
        analysis_destroy(a);
        return NULL;
    }
    int threshold = (int)(gap_factor * a->char_h);
    if (threshold < 2)
        threshold = 2;

    a->root = create_region(0, 0, (int)mat->width - 1,
                            (int)mat->height - 1, 0, CUT_HORIZONTAL);
    segment_region(mat, a->root, CUT_HORIZONTAL, threshold, 0);
    collect_leaves(a->root, &a->leaves, &a->nb_leaves);

    // ---- Macro zones classification ----
    classify_leaves(mat, a->leaves, a->nb_leaves);
    a->grid_region = get_grid_region(a->leaves, a->nb_leaves);
    if (!a->grid_region) {
        analysis_destroy(a);
        return NULL;
    }

    // ---- Grid letters ----
    struct box_list *comps = find_components(mat, a->grid_region->x1,
            a->grid_region->y1, a->grid_region->x2, a->grid_region->y2);
    filter_letter_boxes(comps, a->char_h);
    merge_close_boxes(comps, a->char_h);
    a->grid = build_letter_grid(comps, a->char_h);  // grid owns comps
    check_letter_grid(a->grid);

    // ---- Word lists letters ----
    size_t nb = 0;
    for (size_t i = 0; i < a->nb_leaves; i++)
        if (a->leaves[i]->type == REGION_WORDLIST)
            nb++;
    if (nb > 0) {
        a->wordlists = calloc(nb, sizeof(struct letter_grid *));
        if (!a->wordlists) {
            analysis_destroy(a);
            return NULL;
        }
        for (size_t i = 0; i < a->nb_leaves; i++) {
            if (a->leaves[i]->type != REGION_WORDLIST)
                continue;
            a->wordlists[a->nb_wordlists++] =
                    build_word_list(mat, a->leaves[i]);
        }
    }
    return a;
}

// ===================== Grid accessors =====================

size_t analysis_grid_rows(const struct analysis *a) {
    return a->grid->nb_rows;
}

size_t analysis_grid_cols(const struct analysis *a) {
    // all the rows have the same length (see check_letter_grid)
    return a->grid->nb_rows > 0 ? row_len(a->grid, 0) : 0;
}

struct box analysis_grid_box(const struct analysis *a,
                             size_t row, size_t col) {
    return a->grid->letters->items[a->grid->row_start[row] + col];
}

struct matrix *analysis_grid_letter(const struct analysis *a,
                                    size_t row, size_t col) {
    return letter_to_28(a->mat, analysis_grid_box(a, row, col));
}

// ===================== Words accessors =====================

size_t analysis_nb_words(const struct analysis *a) {
    size_t total = 0;
    for (size_t i = 0; i < a->nb_wordlists; i++)
        total += a->wordlists[i]->nb_rows;
    return total;
}

size_t analysis_word_len(const struct analysis *a, size_t word) {
    size_t row;
    const struct letter_grid *g = locate_word(a, word, &row);
    return g ? row_len(g, row) : 0;
}

struct box analysis_word_box(const struct analysis *a,
                             size_t word, size_t idx) {
    size_t row;
    const struct letter_grid *g = locate_word(a, word, &row);
    if (!g) {
        struct box none = {0, 0, 0, 0};
        return none;
    }
    return g->letters->items[g->row_start[row] + idx];
}

struct matrix *analysis_word_letter(const struct analysis *a,
                                    size_t word, size_t idx) {
    return letter_to_28(a->mat, analysis_word_box(a, word, idx));
}

// ===================== Debug =====================

int analysis_save_debug(const struct analysis *a, SDL_Surface *s) {
    SDL_Surface *rec_regions = SDL_DuplicateSurface(s);
    SDL_Surface *macro = SDL_DuplicateSurface(s);
    SDL_Surface *letters = SDL_DuplicateSurface(s);
    if (!rec_regions || !macro || !letters) {
        SDL_FreeSurface(rec_regions);
        SDL_FreeSurface(macro);      // SDL_FreeSurface(NULL) is safe
        SDL_FreeSurface(letters);
        return -1;
    }
    
    draw_regions(rec_regions, a->root);
    save_result(rec_regions, "recursive_regions.png");

    draw_macro_zones(macro, a->leaves, a->nb_leaves);
    save_result(macro, "macro_zones.png");

    draw_letter_grid(letters, a->grid, REGION_GRID);
    for (size_t i = 0; i < a->nb_wordlists; i++)
        draw_letter_grid(letters, a->wordlists[i], REGION_WORDLIST);
    save_result(letters, "letter_grid.png");

    SDL_FreeSurface(rec_regions);
    SDL_FreeSurface(macro);
    SDL_FreeSurface(letters);
    return 0;
}
