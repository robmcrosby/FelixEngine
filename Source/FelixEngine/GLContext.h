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
#include <map>

class SDL_Window;

namespace fx
{
  class GLWindow;
  
  /**
   *
   */
  class GLContext: public GraphicSystem
  {
  public:
    GLContext();
    virtual ~GLContext();
    
    virtual void update();
    virtual Window* getWindow(const std::string &name);
    
  private:
    std::map<std::string, GLWindow*> mWindows;
  };
}

#endif /* OpenGLSystem_hpp */
