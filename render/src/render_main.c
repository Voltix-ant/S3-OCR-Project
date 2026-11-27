#include "draw.h"
#include "analysis/src/debug_draw.h"
#include "analysis/src/image_io.h"
#include <err.h>

int main(int argc, char **argv) {
    if (argc != 2)
        errx(1, "Incorrect number of parameters");
    
    SDL_Surface *s = load_image(argv[1]);


    draw_highlight(s, 100, 100, 400, 100, 20.0, word_color(0), 0.4);
    draw_highlight(s, 100, 200, 100, 500, 10.0, word_color(1), 0.4);
    draw_highlight(s, 200, 200, 600, 600, 30.0, word_color(2), 0.4);
    draw_highlight(s, 600, 200, 200, 600, 20.0, word_color(3), 0.4);
    save_result(s,"test-render.png");

    
    SDL_FreeSurface(s);


    return 0;
}
