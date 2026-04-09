//
//  SDLGameApp.hpp
//  SDLGame
//
//  Created by Robert Crosby on 3/19/26.
//

#ifndef SDLGameApp_hpp
#define SDLGameApp_hpp

#include "EcsScene.hpp"
#include <SDL3/SDL.h>
#include <iostream>

#include "GpuContext.hpp"
#include "GuiContext.hpp"

class GpuContext;

class SDLGameApp {
private:
  EcsScene mScene;
  uint64_t mLastTime;
  
  GpuContextPtr mGpuContext;
  GuiContextPtr mGuiContext;
  
public:
  SDLGameApp();
  ~SDLGameApp();
  
  SDL_AppResult init();
  SDL_AppResult iterate();
  SDL_AppResult handle(SDL_Event *event);
  void quit(SDL_AppResult result);
};

#endif /* SDLGameApp_hpp */
