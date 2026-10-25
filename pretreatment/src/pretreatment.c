#include "pretreatment.h"

Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
    // Get pixel at coordinates (x, y) of surface.

    if (!surface)
    {
        printf("Error: getting pixel from NULL surface\n");
        return 1;
    }
    else if (surface->format->BytesPerPixel != 4)
    {
        printf("Error: invalid image encoding (must be 4 bytes per pixel)\n");
        return 1;
    }

    Uint32 *pixels_in_32 = (Uint32 *)surface->pixels;
    return pixels_in_32[y * (surface->pitch / 4) + x];
}


int set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    // Set new pixel at coordinates (x, y) of surface.

    if (!surface)
    {
        printf("Error: setting pixel of NULL surface\n");
        return 1;
    }
    else if (surface->format->BytesPerPixel != 4)
    {
        printf("Error: invalid image encoding (must be 4 bytes per pixel)\n");
        return 1;
    }

    Uint32 *pixels_in_32 = (Uint32 *)surface->pixels;
    pixels_in_32[y * (surface->pitch / 4) + x] = pixel;

    return 0;
}


int grayscale(SDL_Surface *surface)
{
    // Converts an image (surface) into grayscale.

	if (!surface) return 0;
	else if (surface->format->BytesPerPixel != 4)
    {
        printf("Error: invalid image encoding (must be 4 bytes per pixel)\n");
        return 1;
    }

	for (int y = 0; y < surface->h; y++)
	{
		for (int x = 0; x < surface->w; x++)
		{
			Uint32 curpixel = get_pixel(surface, x, y);
			Uint8 r, g, b;
			SDL_GetRGB(curpixel, surface->format, &r, &g, &b);

			Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);

			Uint32 new_pixel = SDL_MapRGB(surface->format, gray, gray, gray);
			set_pixel(surface, x, y, new_pixel);
		}
	}

    return 0;
}


int adaptive_threshold(SDL_Surface *surface)
{
    // Converts a grayscale image into black and white.
    // Uses adaptive thresholding ("integral image") to binarize and denoise
    // the resulting surface.

    if (!surface) return 1;
    else if (surface->format->BytesPerPixel != 4)
    {
        printf("Error: invalid image encoding (must be 4 bytes per pixel)\n");
        return 1;
    }

    int w = surface->w;
    int h = surface->h;

    uint64_t *intimg = malloc(w * h * sizeof(uint64_t));
    if (!intimg) return 1;

    // Integral image algorithm
    for (int y = 0; y < h; y++)
    {
        uint64_t sum = 0;
        for (int x = 0; x < w; x++)
        {
            Uint32 curpixel = get_pixel(surface, x, y);
            
            Uint8 r, g, b;
            SDL_GetRGB(curpixel, surface->format, &r, &g, &b);

            sum += r;
            if (y == 0) intimg[x + y * w] = sum;
            else intimg[x + y * w] = intimg[x + (y - 1) * w] + sum;
        }
    }

    // Adaptive threshold application
    int half_zone = w / 32;
    float t = (float)0.15;
    Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);
    Uint32 black = SDL_MapRGB(surface->format, 0, 0, 0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int x1, y1, x2, y2;

            if (x > half_zone) x1 = x - half_zone;
            else x1 = 0;

            if (x + half_zone < w - 1) x2 = x + half_zone;
            else x2 = w - 1;

            if (y > half_zone) y1 = y - half_zone;
            else y1 = 0;

            if (y + half_zone < h - 1) y2 = y + half_zone;
            else y2 = h - 1;

            int count = (x2 - x1 + 1) * (y2 - y1 + 1);
            uint64_t sum = intimg[y2 * w + x2];

            if (x1 > 0 && y1 > 0) sum += intimg[(x1 - 1) + w * (y1 - 1)];
            if (y1 > 0) sum -= intimg[x2 + w * (y1 - 1)];
            if (x1 > 0) sum -= intimg[y2 * w + (x1 - 1)];

            Uint32 curpixel = get_pixel(surface, x, y);
            Uint8 r, g, b;
            SDL_GetRGB(curpixel, surface->format, &r, &g, &b);

            if (r * count < sum * ((double)1.0 - t))
            {
                set_pixel(surface, x, y, black);
            }
            else set_pixel(surface, x, y, white);
        }
    }

    free(intimg);
    return 0;
}

static int gray_at(SDL_Surface *s, int x, int y) {
    /* This function return the color of the pixel of the rotated image, if 
       the pixel is outside the original image, we put a white px */
    if (x < 0 || y < 0 || x >= s->w || y >= s->h) return 255;
    return (get_pixel(s, x, y) >> 16) & 0xFF;
}

SDL_Surface *rotate_surface(SDL_Surface *src, double angle_deg) {
    /* This function rotate the image surface acording an angle, the returned
    surface is bigger than the given one to keep all the informations, part of
    the surface that ar not covered by the original image are put in white*/
    if (!src || src->format->BytesPerPixel != 4) return NULL;

    double a = angle_deg * 3.14159265358979323846 / 180.0;
    double c = cos(a), s = sin(a);
    int w = src->w, h = src->h;
    // Determinate dest surface dimensions
    int nw = (int)ceil(fabs(w * c) + fabs(h * s));
    int nh = (int)ceil(fabs(w * s) + fabs(h * c));

    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(
            0, nw, nh, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!dst) return NULL;

    Uint32 white = SDL_MapRGB(dst->format, 255, 255, 255);
    Uint32 black = SDL_MapRGB(dst->format, 0, 0, 0);
    double cx = w / 2.0, cy = h / 2.0, ncx = nw / 2.0, ncy = nh / 2.0;

    for (int y = 0; y < nh; y++)
    {
        for (int x = 0; x < nw; x++)
        {
            // inverted rotation : destination pixel -> source pixel
            double dx = x + 0.5 - ncx, dy = y + 0.5 - ncy;
            double sx = cx + dx * c + dy * s - 0.5;
            double sy = cy - dx * s + dy * c - 0.5;

            int x0 = (int)floor(sx), y0 = (int)floor(sy);
            double fx = sx - x0, fy = sy - y0;

            double v = gray_at(src, x0,     y0)     * (1 - fx) * (1 - fy)
                     + gray_at(src, x0 + 1, y0)     * fx       * (1 - fy)
                     + gray_at(src, x0,     y0 + 1) * (1 - fx) * fy
                     + gray_at(src, x0 + 1, y0 + 1) * fx       * fy;

            set_pixel(dst, x, y, v < 128 ? black : white);
        }
    }
    return dst;
}

struct matrix *surface_to_matrix(SDL_Surface *s) {
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
            SDL_GetRGBA(get_pixel(s, x, y), s->format, &r, &g, &b, &a);
            mat->data[y][x] = r;
        }
    }
    SDL_UnlockSurface(s);
    return mat;
}
