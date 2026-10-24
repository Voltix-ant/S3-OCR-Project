#include "image_io.h"

SDL_Surface *load_image(const char *path) {
    SDL_Surface *tmp = IMG_Load(path);
    if (!tmp)
        errx(1, "Error : Fail loading image - %s", IMG_GetError());
    // Convert in a universal format : RGBA32
    SDL_Surface *s = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(tmp);
    if (!s)
        errx(1, "Error : Fail to convert the image to RGBA32 - %s", 
                SDL_GetError());
    return s;
}

Uint32 get_px(SDL_Surface *s, int x, int y) {
    return ((Uint32 *)s->pixels)[y * (s->pitch / 4) + x];
}

struct matrix *img_to_matrix(SDL_Surface *s) {
    struct matrix *mat = malloc(sizeof(struct matrix));
    if (!mat)
        errx(1, "img_to_matrix : Couldn't allocate memory");
    mat->width = s->w;
    mat->height = s->h;
    mat->data = malloc(mat->height*sizeof(char *));
    if (!mat->data)
        errx(1, "img_to_matrix : Couldn't allocate memory");
    
    SDL_LockSurface(s);
    for (int y = 0; y < s->h; y++) {
        mat->data[y] = malloc(mat->width*sizeof(unsigned char));
        if (!mat->data[y])
            errx(1, "img_to_matrix : Couldn't allocate memory");
        for (int x = 0; x < s->w; x++) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(get_px(s, x, y), s->format, &r, &g, &b, &a);

            // temporary grayscale transformation function
            // after we will use the one implemented by gustave
            r *= 0.299;
	        g *= 0.587;
	        b *= 0.114;
            Uint8 m = r+g+b;
            mat->data[y][x] = m;
        }
    }
    SDL_UnlockSurface(s);
    return mat;
}

void binarize_matrix(struct matrix *m, size_t block_size, char C) {
    // Integral image for optimizarion : I[y][x] = sum of all the px in the
    // square (0,0) to (x,y)
    Uint64 **I = calloc(m->height,sizeof(Uint64 *));
    for (size_t y = 0; y < m->height; y++) {
        I[y] = calloc(m->width,sizeof(Uint64));
        Uint64 row = 0;
        for (size_t x = 0; x < m->width; x++) {
            row += m->data[y][x];
            if (y != 0) {
                I[y][x] += I[y-1][x];
            }
            I[y][x] += row;
        }
    }

    size_t r = block_size / 2;
    // For each pixel in the matrix
    for (size_t y = 0; y < m->height; y++) {
        size_t y1 = (y > r) ? y - r : 0;
        size_t y2 = (y + r + 1 < m->height) ? y + r + 1 : m->height-1;
        for (size_t x = 0; x < m->width; x++) {
            size_t x1 = (x > r) ? x - r : 0;
            size_t x2 = (x + r + 1 < m->width) ? x + r + 1 : m->width-1;
            // We determinate the coordinate (x1,y1) (x2,y2) of the square
            // We calculate the corresponding sum : 
            // I[y2][x2] − I[y1][x2] − I[y2][x1] + I[y1][x1]
            uint64_t sum = I[y2][x2] - I[y1][x2] - I[y2][x1] + I[y1][x1];
            // We calculate the mean of the square :
            int area = (x2 - x1) * (y2 - y1);
            unsigned char mean = (unsigned char)(sum / area);
            
            m->data[y][x] = (m->data[y][x] > mean - C) ? 255 : 0;
        }
    }
    for (size_t y = 0; y < m->height; y++) {
        free(I[y]);
    }
    free(I);
}

static void set_px(SDL_Surface *s, int x, int y, Uint32 color)
{
    ((Uint32 *)s->pixels)[y * (s->pitch / 4) + x] = color;
}

SDL_Surface *matrix_to_img(struct matrix *m) {
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, 
            m->width, m->height, 32, SDL_PIXELFORMAT_RGBA32);
    for (size_t y = 0; y < m->height; y++) {
        for (size_t x = 0; x < m->width; x++) {
            Uint8 v = (Uint8) m->data[y][x];
            Uint32 pxf = SDL_MapRGB(s->format, v, v, v);
	        set_px(s, x, y, pxf);
        }
    }
    return s;
}

