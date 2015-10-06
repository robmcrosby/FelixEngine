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
    
    virtual bool loadResizable(const std::string &title, ivec2 size);
    virtual bool loadResizable(const std::string &title, ivec2 size, ivec2 pos);
    
    void setActive();
    void swapBuffers();
    
  private:
    SDL_Window *mSDLWindow;
    GLContext  *mGLContext;
  };
}



#endif /* GLWindow_hpp */
