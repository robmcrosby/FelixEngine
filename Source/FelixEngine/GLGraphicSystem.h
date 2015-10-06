//
//  GLGraphicSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLGraphicSystem_h
#define GLGraphicSystem_h

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
  class GLGraphicSystem: public GraphicSystem
  {
  public:
    GLGraphicSystem();
    virtual ~GLGraphicSystem();
    
    virtual void update();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual void setVersion(int major, int minor);
    virtual Window* getWindow(const std::string &name);
    
    void setContext(SDL_GLContext context) {mContext = context;}
    SDL_GLContext getContext() {return mContext;}
    
  private:
    bool setVersion(const XMLTree::Node *node);
    
    std::map<std::string, GLWindow*> mWindows;
    SDL_GLContext mContext;
  };
}

#endif /* OpenGLSystem_hpp */
