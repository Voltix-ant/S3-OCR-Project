#include "ui/src/ui.h"
#include "pretreatment/src/pretreatment.h"
#include "common/src/matrix.h"
#include "analysis/src/analysis.h"
#include "analysis/src/debug_draw.h"


int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 2)
	{
			printf("Usage: ./ocr [filepath]\n");
			return 1;
	}

    // App window and renderer creation
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (init_window(&window, &renderer) != 0) return 1;
    SDL_SetWindowMinimumSize(window, 960, 540);
    int win_w = 1920;
    int win_h = 1080;
    
	// Creation of the zones
   	SDL_Rect panel = create_panel_rect(win_w, win_h);
 	SDL_Rect preview = create_preview_rect(win_w, win_h);
    
    // Creation of the buttons and the rotation device
    RotationDevice slider = create_rotation_device(panel);

	// Loading image in SDL_Surface
	SDL_Surface *img = IMG_Load(argv[1]);
	if (!img)
	{
		printf("Error: failed to load image: %s\n", IMG_GetError());
    	terminate(window, renderer);
		IMG_Quit();
		return 1;
	}

	SDL_Surface *og_img = SDL_ConvertSurfaceFormat(
		img,
		SDL_PIXELFORMAT_ARGB8888,
		0
		);
	SDL_FreeSurface(img);

	if (!og_img)
	{
		printf("Error: failed image conversion: %s\n", SDL_GetError());
    	terminate(window, renderer);
		IMG_Quit();
		return 1;
	}

	// Application of the pretreatment functions
	grayscale(og_img);
	adaptive_threshold(og_img);

	// Creation of the rendering texture
	SDL_Texture *og_tex = SDL_CreateTextureFromSurface(renderer, og_img);
	if (!og_tex)
	{
		printf("Error: couldn't create image texture\n");
    	terminate(window, renderer);
		IMG_Quit();
		return 1;
	}

    // Retrieving image dimensions
    int img_w, img_h;
    SDL_QueryTexture(og_tex, NULL, NULL, &img_w, &img_h);

	// Creating all the interface's buttons
	int nb_buttons = 2;
	Button buttons[nb_buttons];
	// "Save image" button
	buttons[0].is_active = 0; // Saving an image can only be done after analysis
	buttons[0].is_hovered = 0;
	buttons[0].text = IMG_LoadTexture(renderer, "ui/assets/save_image.png");
	if (!buttons[0].text)
	{
		printf("Error loading button texture: %s\n", IMG_GetError());
		SDL_DestroyTexture(og_tex);
		terminate(window, renderer);
		IMG_Quit();
		return 1;
	}
	// "Start analysis" button
	buttons[1].is_active = 1;
	buttons[1].is_hovered = 0;
	buttons[1].text = IMG_LoadTexture(renderer, "ui/assets/start_analysis.png");
	if (!buttons[1].text)
	{
		printf("Error loading button texture: %s\n", IMG_GetError());
		SDL_DestroyTexture(buttons[0].text);
		SDL_DestroyTexture(og_tex);
		terminate(window, renderer);
		IMG_Quit();
		return 1;
	}
	update_buttons_pos(buttons, panel); // initialize button positions

	// MAIN LOOP
	int running = 1;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Retrieval of the events
			if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_WINDOWEVENT &&
                    event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                win_w = event.window.data1;
                win_h = event.window.data2;

	        	// Update the zones according to the new window dimensions
   	        	panel = create_panel_rect(win_w, win_h);
 	        	preview = create_preview_rect(win_w, win_h);

                // Update the slider's position 
                slider.axis.x = panel.x + panel.w / 10;
                slider.axis.y = panel.y + panel.h / 4;
                slider.axis.w = panel.w * 8 / 10;

                int mx = slider.axis.x + (
					(int)slider.axis.w*(slider.angle+180)/360
				);
                slider.cursor.x = mx - slider.cursor.w / 2;
                slider.cursor.y = slider.axis.y - 14;

				update_buttons_pos(buttons, panel);
            }

			// Update slider and buttons
            update_slider(&slider, &event);
			int btn_index = update_buttons(buttons, nb_buttons, &event);
			if (btn_index == 0) // SAVE IMAGE
			{
				printf("DEBUG: \"Save image\" button clicked.\n"); // TO REPLACE
			}
			else if (btn_index == 1) // START ANALYSIS
			{
				// Rotate the surface
                SDL_Surface *rot = rotate_surface(og_img, slider.angle);
                // Translate it to a matrix
                struct matrix *mat = surface_to_matrix(rot);
                // Save an image of the rotated version
                save_result(rot,"rotation.png");

                // run analysis
                struct analysis *a = analysis_run(mat);
                if (!a) {
                    errx(1,"Analysis failed...");
                }
                // Save debug images
                analysis_save_debug(a, rot);
                
                analysis_destroy(a);
                destroy_matrix(mat);
                SDL_FreeSurface(rot);
			}
		}

		// RENDERING
		// NOTE: remember to get the current window size
		// to adapt the rendering scales

    	// Setting the background to very dark grey
   		SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
   		SDL_RenderClear(renderer);

        // Display panel and preview zones
        display_panel(renderer, panel);
        display_preview(renderer, preview);

        // Display buttons and slider
		display_buttons(renderer, buttons, nb_buttons);
        display_slider(renderer, slider);

		// Display of the preview image
		// NOTE: this is also where the 'result' image should be displayed,
		// so remember to check which one has to be displayed currently
		// (buttons to choose to display 'preview' or 'result'!!!)
        SDL_Rect fitted_preview = get_fitted_rect(preview, img_w, img_h);
        SDL_RenderSetClipRect(renderer, &preview);
        SDL_RenderCopyEx(
			renderer,
            og_tex,
            NULL,
            &fitted_preview,
            (double)slider.angle,
            NULL,
            SDL_FLIP_NONE
            );
		display_preview_grid(renderer, preview);
        SDL_RenderSetClipRect(renderer, NULL);

		SDL_RenderPresent(renderer);

        SDL_Delay(1000/60);
    }

	SDL_DestroyTexture(og_tex);
	IMG_Quit();
	SDL_FreeSurface(og_img);
    terminate(window, renderer);

    return 0;
}
