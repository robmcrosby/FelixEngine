//
//  GLWindow.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLWindow_hpp
#define GLWindow_hpp


#include "GraphicSystem.h"


class SDL_Window;

namespace fx
{
  class GLContext;
  
  /**
   *
   */
  class GLWindow: public Window
  {
  public:
    GLWindow(GLContext *context);
    virtual ~GLWindow();
    
    virtual bool load();
    
    virtual void setTitle(const std::string &title);
    virtual void setPosition(const ivec2 &pos);
    virtual void setSize(const ivec2 &size);
    
    void setActive();
    void swapBuffers();
    
  private:
    ivec2 mPos, mSize;
    std::string mTitle;
    SDL_Window *mSDLWindow;
    GLContext  *mGLContext;
  };
}



#endif /* GLWindow_hpp */
