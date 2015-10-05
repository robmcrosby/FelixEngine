//
//  OpenGLContext.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLContext_h
#define GLContext_h

#include "GraphicSystem.h"

class SDL_Window;

namespace fx
{
  struct GLWindow: public Window
  {
    SDL_Window *mWindow;
  };
  
  /**
   *
   */
  class GLContext: public GraphicSystem
  {
  public:
    GLContext();
    virtual ~GLContext();
    
    virtual void update();
    virtual Window* createResizableWindow();
  };
}

#endif /* OpenGLSystem_hpp */
