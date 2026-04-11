//
//  BaseGameApp.hpp
//  BaseGame
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef BaseGameApp_hpp
#define BaseGameApp_hpp

#include <FelixEngine/GpuContext.hpp>
#include <FelixEngine/GuiContext.hpp>
#include <SDL3/SDL.h>
#include <iostream>

class BaseGameApp {
private:
  Felix::Scene mScene;
  Felix::GpuContextPtr mGpuContext;
  Felix::GuiContextPtr mGuiContext;
  
public:
  BaseGameApp();
  ~BaseGameApp();

  SDL_AppResult init();
  SDL_AppResult iterate();
  SDL_AppResult handle(SDL_Event *event);
  void quit(SDL_AppResult result);
};



#endif /* BaseGameApp_hpp */
