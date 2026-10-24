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

            if (r * count < sum * ((float)1.0 - t))
            {
                set_pixel(surface, x, y, black);
            }
            else set_pixel(surface, x, y, white);
        }
    }

    free(intimg);
    return 0;
}
