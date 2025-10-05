#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <libgen.h>
#include "pic.h"

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    pic_t pic;
    u64 ticks;
    bool needs_redraw;
} app_t;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (argc < 2)
    {
        SDL_Log("Usage: ./bild [path]");
        return SDL_APP_FAILURE;
    }

    u8 *buffer = malloc(sizeof(u8) * 10485760);
    size_t buffer_size = read_file(argv[1], buffer);
    free(buffer);
    pic_t pic = pic_decode(buffer, buffer_size);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Could not initialize SDL: %s", SDL_GetError());
        pic_free(pic);
        return SDL_APP_FAILURE;
    }

    app_t *app = malloc(sizeof(app_t));
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    char *name = basename(argv[1]);

    if (!SDL_CreateWindowAndRenderer(name, pic.width, pic.height, flags, &app->window, &app->renderer))
    {
        SDL_Log("Could not create window/renderer: %s", SDL_GetError());
        pic_free(pic);
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(app->renderer, 1);
    SDL_Texture *texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, pic.width, pic.height);

    if (texture == NULL)
    {
        SDL_Log("Could not create texture: %s", SDL_GetError());
        pic_free(pic);
        return SDL_APP_FAILURE;
    }

    bool ok = SDL_UpdateTexture(texture, NULL, pic.data, pic.width * 4);
    if (!ok)
    {
        SDL_Log("Could not update texture: %s", SDL_GetError());
        pic_free(pic);
        return SDL_APP_FAILURE;
    }

    app->texture = texture;
    app->pic = pic;
    app->needs_redraw = true;

    *appstate = app;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{    
    app_t *app = appstate;

    switch (event->type)
    {
    case SDL_EVENT_KEY_DOWN:
    {
        if (event->key.key == SDLK_ESCAPE)
        {
            return SDL_APP_SUCCESS;
        }
        break;
    }
    case SDL_EVENT_WINDOW_RESIZED:
        app->needs_redraw = true;
        break;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    app_t *app = appstate;

    if (!app->needs_redraw)
    {
        // not sure if this is kosher.
        SDL_Delay(1);
        return SDL_APP_CONTINUE;
    }

    u64 ticks = SDL_GetTicks();
    bool do_show_fps = SDL_floor(ticks / 1000.0) != SDL_floor(app->ticks / 1000.0);

    if (do_show_fps)
    {
        u64 delta = ticks - app->ticks;
        f64 fps = 1000.0 / ((f64)delta);
        SDL_Log("FPS: %.2f\n", fps);
    }

    app->ticks = ticks;

    i32 render_w;
    i32 render_w_scaled;
    i32 render_h;
    i32 render_h_scaled;

    SDL_GetRenderOutputSize(app->renderer, &render_w, &render_h);

    render_w_scaled = render_w;
    render_h_scaled = render_h;

    f64 render_aspect_ratio = ((f64)render_w) / render_h;
    f64 bmp_aspect_ratio = ((f64)app->pic.width) / app->pic.height;

    // center the image and preserve its aspect ratio.
    if (render_aspect_ratio > bmp_aspect_ratio)
    {
        render_w_scaled = render_h * bmp_aspect_ratio;
    }
    else
    {
        render_h_scaled = render_w / bmp_aspect_ratio;
    }

    SDL_FRect rect = {
        .x = (render_w - render_w_scaled) / 2,
        .y = (render_h - render_h_scaled) / 2,
        .w = render_w_scaled,
        .h = render_h_scaled,
    };

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_RenderTexture(app->renderer, app->texture, NULL, &rect);
    SDL_RenderPresent(app->renderer);

    app->needs_redraw = false;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate == NULL)
    {
        return;
    }

    app_t *app = appstate;
    SDL_DestroyTexture(app->texture);
    pic_free(app->pic);
    free(app);
}
