#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

// Button object for every button in the UI.
typedef void (*ButtonClickFunction)(void);
typedef struct {
    SDL_Rect rect; // button rectangle
    char *text;
    SDL_Color text_color;
    SDL_Color bg_color;
    SDL_Color hover_color;
    int is_active; // 1 if the associated function is useful at the moment
    ButtonClickFunction function; // associated function
} Button;

// RotationDevice object: a cursor to move and its axis, especially for the
// manual rotation.
typedef struct {
    SDL_Rect cursor;
    SDL_Rect axis; // the cursor's axis
    int is_dragging; // 1 if user is dragging the cursor
    int angle;
} RotationDevice;


int init_window(SDL_Window **window, SDL_Renderer **renderer);
void terminate(SDL_Window *window, SDL_Renderer *renderer);

SDL_Rect create_panel_rect(int win_w, int win_h);
SDL_Rect create_preview_rect(int win_w, int win_h);
void display_panel(SDL_Renderer *renderer, SDL_Rect panel);
void display_preview(SDL_Renderer *renderer, SDL_Rect preview);
SDL_Rect get_fitted_rect(SDL_Rect preview, int img_w, int img_h);

RotationDevice create_rotation_device(SDL_Rect panel);
void update_slider(RotationDevice *slider, SDL_Event *event);
void display_slider(SDL_Renderer *renderer, RotationDevice slider);

#endif

