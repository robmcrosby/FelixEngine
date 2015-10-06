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

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

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
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual void setVersion(int major, int minor);
    virtual Window* getWindow(const std::string &name);
    
    void setContext(SDL_GLContext context) {mSDL_GLContext = context;}
    SDL_GLContext getContext() {return mSDL_GLContext;}
    
  private:
    bool setVersion(const XMLTree::Node *node);
    
    std::map<std::string, GLWindow*> mWindows;
    SDL_GLContext mSDL_GLContext;
  };
}

#endif /* OpenGLSystem_hpp */
