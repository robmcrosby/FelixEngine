//
//  BaseGameApp.cpp
//  BaseGame
//
//  Created by Robert Crosby on 4/9/26.
//

#include "BaseGameApp.hpp"


using namespace std;

BaseGameApp::BaseGameApp() {
  
}

BaseGameApp::~BaseGameApp() {
  
}

SDL_AppResult BaseGameApp::init() {
  // Initalize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    return SDL_APP_FAILURE;
  }
  
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::iterate() {
  return SDL_APP_CONTINUE;
}

SDL_AppResult BaseGameApp::handle(SDL_Event *event) {
  return SDL_APP_CONTINUE;
}

void BaseGameApp::quit(SDL_AppResult result) {
  SDL_Quit();
}
