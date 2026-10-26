#include "ui.h"

/* NOTE
 * All the UI functions are designed to automatically adapt to the window size.
*/

SDL_Rect create_panel_rect(int win_w, int win_h)
{
    // Creates the control panel's rectangular zone which contains
    // the main buttons: save image, rotation cursor and start solver,
    // and returns the new zone's rect.

    SDL_Rect panel_rect = {
        .x = 0.75 * win_w,
        .y = win_h / 25,
        .w = 0.23 * win_w,
        .h = win_h * 23 / 25
    };
    
    return panel_rect;
}

SDL_Rect create_preview_rect(int win_w, int win_h)
{
    // Creates the frame for the display of the preview and the resulting
    // solved grid, and returns the rect corresponding to it.

    SDL_Rect preview_rect = {
        .x = 0.02 * win_w,
        .y = win_h / 25,
        .w = 0.72 * win_w,
        .h = win_h * 21 / 25
    };

    return preview_rect;
}

RotationDevice create_rotation_device(SDL_Rect panel)
{
    // Creates the rotation slider with its axis to rotate the image manually,
    // and returns the new cursor.

    // cursor's axis
    SDL_Rect axis = {
        .x = panel.x + panel.w / 10,
        .y = panel.y + panel.h / 4,
        .w = panel.w * 8 / 10,
        .h = 4
    };

    // cursor itself
    SDL_Rect cursor = {
        .x = axis.x + axis.w / 2 - 16,
        .y = axis.y - 14,
        .w = 32,
        .h = 32
    };

    // new rotation device
    RotationDevice new = {
        .cursor = cursor,
        .axis = axis,
        .is_dragging = 0,
        .angle = 0
    };

    return new;
}

void update_slider(RotationDevice *slider, SDL_Event *event)
{
    // Updates the rotation device.

    if (event->type == SDL_MOUSEBUTTONDOWN &&
        event->button.button == SDL_BUTTON_LEFT)
    {
        // if user is pressing left click
        int bx = event->button.x;
        int by = event->button.y;

        // if mouse on cursor, we have to drag the cursor accordingly
        if (bx >= slider->cursor.x &&
            bx <= slider->cursor.x + slider->cursor.w &&
            by >= slider->cursor.y &&
            by <= slider->cursor.y + slider->cursor.h)
        {
            slider->is_dragging = 1;
        }
    }
    else if (event->type == SDL_MOUSEBUTTONUP &&
            event->button.button == SDL_BUTTON_LEFT)
    {
        // the user is not left-clicking, we cannot move the cursor
        slider->is_dragging = 0;
    }
    else if (event->type == SDL_MOUSEMOTION && slider->is_dragging)
    {
        // if mouse moved and cursor being dragged, move the cursor accordingly
        int mx = event->motion.x;

        if (mx < slider->axis.x) mx = slider->axis.x;
        if (mx > slider->axis.x + slider->axis.w)
        {
            mx = slider->axis.x + slider->axis.w;
        }

        slider->cursor.x = mx - slider->cursor.w / 2;

        // updating slider's angle
        slider->angle = 360 * (mx - slider->axis.x) / slider->axis.w - 180;
    }
}

void display_panel(SDL_Renderer *renderer, SDL_Rect panel)
{
    // Displays the control panel.

    SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255); 
    SDL_RenderFillRect(renderer, &panel);
}

void display_preview(SDL_Renderer *renderer, SDL_Rect preview)
{
    // Displays the preview/result zone's rectangle.

    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    SDL_RenderFillRect(renderer, &preview);
}

void display_slider(SDL_Renderer *renderer, RotationDevice slider)
{
    // Displays the cursor and its axis in the panel.

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &(slider.axis));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &(slider.cursor));
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &(slider.cursor));
}

SDL_Rect get_fitted_rect(SDL_Rect preview, int img_w, int img_h)
{
    // Gives an adapted Rect shape to display the preview without stretching
    // the image.

    SDL_Rect fitted;

    if (preview.w * img_h < preview.h * img_w)
    {
        fitted.w = preview.w;
        fitted.h = img_h * preview.w / img_w;
    }
    else
    {
        fitted.w = img_w * preview.h / img_h;
        fitted.h = preview.h;
    }

    fitted.x = preview.x + (preview.w - fitted.w) / 2;
    fitted.y = preview.y + (preview.h - fitted.h) / 2;

    return fitted;
}

void update_buttons_pos(Button *buttons, SDL_Rect panel)
{
    // Updates the buttons' positions and shapes when the window gets resized.

    int btn_w = panel.w * 8 / 10;
    int btn_h = panel.h * 8 / 100;
    int btn_x = panel.x + (panel.w - btn_w) / 2;

    // "Save image" button
    buttons[0].rect.w = btn_w;
    buttons[0].rect.h = btn_h;
    buttons[0].rect.x = btn_x;
    buttons[0].rect.y = panel.y + panel.h / 2 - btn_h / 2;

    // "Start analysis" button
    buttons[1].rect.w = btn_w;
    buttons[1].rect.h = btn_h;
    buttons[1].rect.x = btn_x;
    buttons[1].rect.y = panel.y + panel.h * 3/4 - btn_h / 2;
}

void update_buttons(Button *buttons, int nb_btns, SDL_Event *event)
{
    // Updates each button in the array buttons and executes their associated
    // functions if necessary.

    for (int i = 0; i < nb_btns; i++)
    {
        Button *b = &buttons[i];
        
        if (event->type == SDL_MOUSEMOTION)
        {
            int mx = event->motion.x;
            int my = event->motion.y;

            if (b->rect.x <= mx && mx <= b->rect.x + b->rect.w &&
                b->rect.y <= my && my <= b->rect.y + b->rect.h)
            {
                b->is_hovered = 1;
            }
            else
            {
                b->is_hovered = 0;
            }
        }
        else if (event->type == SDL_MOUSEBUTTONUP &&
                 event->button.button == SDL_BUTTON_LEFT)
        {
            if (b->is_hovered && b->is_active)
            {
                b->function();
            }
        }
    }
}

void display_buttons(SDL_Renderer *renderer, Button *buttons, int nb_btns)
{
    // Displays every button according to their shape and current position.
    // Also displays their associated text, if any.

    for (int i = 0; i < nb_btns; i++)
    {
        if (!buttons[i].is_active)
        {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        }
        else if (!buttons[i].is_hovered)
        {
            SDL_SetRenderDrawColor(renderer, 130, 160, 130, 255);
        }
        else SDL_SetRenderDrawColor(renderer, 155, 185, 155, 255);

        SDL_RenderFillRect(renderer, &buttons[i].rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &buttons[i].rect);

        if (buttons[i].text != NULL)
        {
            int tex_w, tex_h;
            SDL_QueryTexture(buttons[i].text, NULL, NULL, &tex_w, &tex_h);
            int l = buttons[i].rect.w * 1 / 10;
            SDL_Rect adapted_rect = { // add a limit to make the text cleaner
                .x = buttons[i].rect.x + l,
                .y = buttons[i].rect.y,
                .w = buttons[i].rect.w - 2*l,
                .h = buttons[i].rect.h
            };
            SDL_Rect text_rect = get_fitted_rect(adapted_rect, tex_w, tex_h);
            SDL_RenderCopy(renderer, buttons[i].text, NULL, &text_rect);
        }
    }
}