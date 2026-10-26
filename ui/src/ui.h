#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>


typedef struct {
    SDL_Rect rect; // button rectangle
    int is_active; // 1 if the associated function is useful at the moment
    int is_hovered; // 1 if the mouse cursor is on the button
    void (*function)(void); // associated function
    SDL_Texture *text; // The transparent image containing the button's text
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
void display_preview_grid(SDL_Renderer *renderer, SDL_Rect preview);
SDL_Rect get_fitted_rect(SDL_Rect preview, int img_w, int img_h);

RotationDevice create_rotation_device(SDL_Rect panel);
void update_slider(RotationDevice *slider, SDL_Event *event);
void display_slider(SDL_Renderer *renderer, RotationDevice slider);

void update_buttons_pos(Button *buttons, SDL_Rect panel);
void update_buttons(Button *buttons, int nb_btns, SDL_Event *event);
void display_buttons(SDL_Renderer *renderer, Button *buttons, int nb_btns);

#endif

