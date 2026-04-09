#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <iostream>
#include "SDLGameApp.hpp"


using namespace std;

/**
 * Called once when the application starts for initialization.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  SDLGameApp *state = new SDLGameApp();
  if (!state) {
    cerr << "Error Instancing SDLGameApp!" << endl;
    return SDL_APP_FAILURE;
  }
  
  *appstate = state;
  return state->init();
}

/**
 * Called repeatedly, possibly at the display's refresh rate, for rendering and updates.
 */
SDL_AppResult SDL_AppIterate(void *appstate) {
  SDLGameApp* state = (SDLGameApp*)appstate;
  return state->iterate();
}

/**
 * Called when an event arrives.
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  SDLGameApp* state = (SDLGameApp*)appstate;
  return state->handle(event);
}

/**
 * Called once when the application is quitting.
 */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  SDLGameApp* state = (SDLGameApp*)appstate;
  if (state) {
    state->quit(result);
    delete state;
  }
  cout << "Application quit with result: " << result << endl;
}
