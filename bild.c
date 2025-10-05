#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <stdio.h>
#include "pic.h"

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    u64 ticks;
} app_t;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    app_t *app = malloc(sizeof(app_t));
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (!SDL_CreateWindowAndRenderer("bild", 640, 480, flags, &app->window, &app->renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(app->renderer, 1);

    *appstate = app;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_EVENT_KEY_DOWN:
    {
        if (event->key.key == SDLK_ESCAPE)
        {
            return SDL_APP_SUCCESS;
        }
    }
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    app_t *app = appstate;

    u64 ticks = SDL_GetTicks();
    bool do_show_fps = SDL_floor(ticks / 1000.0) != SDL_floor(app->ticks / 1000.0);

    if (do_show_fps)
    {
        u64 delta = ticks - app->ticks;
        f64 fps = 1000.0 / ((f64)delta);
        printf("FPS: %.2f\n", fps);
    }

    app->ticks = ticks;

    const double now = ((double)app->ticks) / 1000.0;
    const float red = (float)(0.5 + 0.5 * SDL_sin(now));
    const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

    SDL_SetRenderDrawColorFloat(app->renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    app_t *app = appstate;
    free(app);
}
