#include <stdio.h>
#include <err.h>
#include "image_io.h"
#include "analysis.h"
#include "common/src/matrix.h"

int main(int argc, char **argv) {
    if (argc != 2)
        errx(1, "Incorrect number of parameters");

    // Generate matrix from the image (replace preproceesing)
    SDL_Surface *s = load_image(argv[1]);
    struct matrix *mat = img_to_matrix(s);

    // ruu the actual analysis
    struct analysis *a = analysis_run(mat);
    if (!a)
        errx(1, "Analysis failed : no grid found");

    // debug display
    printf("grid : %zu rows x %zu cols\n",
           analysis_grid_rows(a), analysis_grid_cols(a));
    for (size_t w = 0; w < analysis_nb_words(a); w++)
        printf("word %zu : %zu letters\n", w, analysis_word_len(a, w));

    // Create debug_images
    analysis_save_debug(a, s);

    // Destroy everything
    analysis_destroy(a);
    destroy_matrix(mat);
    SDL_FreeSurface(s);
    return 0;
}
