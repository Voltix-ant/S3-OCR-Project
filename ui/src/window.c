#include "ui.h"

int init_window(SDL_Window **window, SDL_Renderer **renderer)
{
    // Creates the app's window and renderer

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init() failure: %s\n", SDL_GetError());
        return 1;
    }

	*window = SDL_CreateWindow(
            "Wordsearch solver",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1920,
            1080,
            SDL_WINDOW_RESIZABLE
            );

    if (!window)
    {
        printf("SDL_CreateWindow() failure: %s\n", SDL_GetError());
        return 1;
    }

	*renderer = SDL_CreateRenderer(
            *window,
            -1,
            SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC
            );

    if (!renderer)
    {
        printf("SDL_CreateRenderer() failure: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        return 1;
    }

    return 0;
}

void terminate(SDL_Window *window, SDL_Renderer *renderer)
{
    // Closes the window and the renderer

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

