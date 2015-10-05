//
//  OpenGLSystem.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef OpenGLSystem_h
#define OpenGLSystem_h

#include "GpuSystem.h"

class SDL_Window;

namespace fx
{
  struct GLWindow: public GPUWindow
  {
    SDL_Window *mSdlWindow;
  };
  
  /**
   *
   */
  class OpenGLSystem: public GpuSystem
  {
  public:
    OpenGLSystem();
    virtual ~OpenGLSystem();
    
    virtual GPUWindow* createResizableWindow();
    
    virtual void update();
  };
}

#endif /* OpenGLSystem_hpp */
