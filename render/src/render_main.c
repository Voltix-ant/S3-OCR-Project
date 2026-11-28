#include "solution_image.h"
#include "analysis/src/debug_draw.h"
#include "analysis/src/image_io.h"
#include "analysis/src/analysis.h"
#include "common/src/word_result.h"
#include <err.h>

int main(void) {
    
    SDL_Surface *s = load_image("img_test/lvl1-img1.png");
    struct matrix *mat = img_to_matrix(s);
    
    binarize_matrix(mat, mat->width/20, 10);

    // run the actual analysis
    struct analysis *a = analysis_run(mat);
    if (!a)
        errx(1, "Analysis failed : no grid found");

    struct found_word words[3] = {
        {1,3,1,8,1},
        {1,2,3,2,12},
        {1,4,4,10,10}
    };

    draw_solution(s, a, words, 3);

    save_result(s,"test-render.png");

    
    analysis_destroy(a);
    destroy_matrix(mat);
    SDL_FreeSurface(s);


    return 0;
}
