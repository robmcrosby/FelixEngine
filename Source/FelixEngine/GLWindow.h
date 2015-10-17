//
//  GLWindow.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLWindow_hpp
#define GLWindow_hpp


#include "GLGraphicSystem.h"


class SDL_Window;

namespace fx
{
  class GLFrame;
  
  /**
   *
   */
  class GLWindow: public Window
  {
  public:
    GLWindow(GLGraphicSystem *system, const std::string &name);
    virtual ~GLWindow();
    
    bool init()
    {
      bool success = load();
      if (success)
        setLoaded();
      else
        setNotLoading();
      return success;
    }
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    
    virtual void setTitle(const std::string &title);
    virtual void setPosition(const ivec2 &pos);
    virtual void setSize(const ivec2 &size);
    
    void setActive();
    void swapBuffers();
    
  private:
    bool load();
    
    std::string mName, mTitle;
    SDL_Window *mSDLWindow;
    GLFrame    *mGLFrame;
    GLGraphicSystem *mGLSystem;
  };
}



#endif /* GLWindow_hpp */
