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
  /**
   *
   */
  class GLWindow: public Window
  {
  public:
    GLWindow();
    virtual ~GLWindow();
    
    virtual bool loadResizable(const std::string &title, ivec2 pos, ivec2 size);
    
    void swapBuffers();
    
  private:
    SDL_Window *mSDLWindow;
  };
}



#endif /* GLWindow_hpp */
