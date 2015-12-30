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
    GLWindow(GLGraphicSystem *system): mGLSystem(system), mSDLWindow(0), mFrameBufferId(0) {}
    virtual ~GLWindow()
    {
      if (mSDLWindow)
        SDL_DestroyWindow(mSDLWindow);
    }
    
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
      if (loading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
      
      if (loaded())
      {
        SDL_GL_GetDrawableSize(mSDLWindow, &mSize.w, &mSize.h);
        
        #if __IPHONEOS__
        updateFrameBufferId();
        #endif
      }
    }
    
    void updateFrameBufferId()
    {
      SDL_GL_MakeCurrent(mSDLWindow, mGLSystem->getContext());
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mFrameBufferId);
    }
    
    void setActive(int stereo)
    {
      if (mSDLWindow)
      {
        ivec2 pos;
        ivec2 size(mSize);
        
        // Adjust the Viewport for Left and Right drawing
        if (mMode == WINDOW_LEFT_RIGHT || mMode == WINDOW_RIGHT_LEFT)
        {
          size.x /= 2;
          if ((mMode == WINDOW_LEFT_RIGHT && stereo == STEREO_LEFT) ||
              (mMode == WINDOW_RIGHT_LEFT && stereo == STEREO_RIGHT))
            pos.x = size.x;
        }
        else if (mMode == WINDOW_LEFT_OVER_RIGHT || mMode == WINDOW_RIGHT_OVER_LEFT)
        {
          size.y /= 2;
          if ((mMode == WINDOW_LEFT_OVER_RIGHT && stereo == STEREO_LEFT) ||
              (mMode == WINDOW_RIGHT_OVER_LEFT && stereo == STEREO_RIGHT))
            pos.y = size.y;
        }
        
        SDL_GL_MakeCurrent(mSDLWindow, mGLSystem->getContext());
        
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
        glViewport(pos.x, pos.y, size.w, size.h);
        glScissor(pos.x, pos.y, size.w, size.h);
        glEnable(GL_SCISSOR_TEST);
      }
    }
    
    void swapBuffers()
    {
      if (mSDLWindow)
        SDL_GL_SwapWindow(mSDLWindow);
    }
    SDL_Window* getSDLWindow() {return mSDLWindow;}
    
    GLuint frameBufferId() const {return mFrameBufferId;}
    
  private:
    bool load()
    {
      bool success = false;
      if (!loaded())
      {
        #if __IPHONEOS__
        // Get the Screen Size.
        SDL_DisplayMode displayMode;
        SDL_GetDesktopDisplayMode(0, &displayMode);
        
        mSize.w = displayMode.w;
        mSize.h = displayMode.h;
        mScale = 2.0f;
        
        mSDLWindow = SDL_CreateWindow(NULL, 0, 0, mSize.w, mSize.h,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN |
                                      SDL_WINDOW_RESIZABLE);

        #else
        // Get and check the SDL Window
        mSDLWindow = SDL_CreateWindow(mTitle.c_str(),
                                      mPosition.x, mPosition.y,
                                      mSize.w, mSize.h,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
                                      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        
        if (mSDLWindow)
        {
          ivec2 size;
          SDL_GL_GetDrawableSize(mSDLWindow, &size.w, &size.h);
          mScale = (float)size.w / (float)mSize.w;
          mSize = size;
        }
        
        #endif
        
        
        if (!mSDLWindow)
        {
          std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
          return false;
        }
        
        // Check if the OpenGL Context has been created already.
        if (!mGLSystem->getContext())
        {
          // Create and check the OpenGL Context
          SDL_GLContext context = SDL_GL_CreateContext(mSDLWindow);
          if (!context)
          {
            std::cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
            return false;
          }
          mGLSystem->setContext(context);
          
          // Set to Use V-Sync
          #if !__IPHONEOS__
          if (SDL_GL_SetSwapInterval(1) < 0)
            std::cerr << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
          #endif
        }
        success = true;
      }
      return success;
    }
    
    SDL_Window *mSDLWindow;
    GLGraphicSystem *mGLSystem;
    GLint mFrameBufferId;
  };
}



#endif /* GLWindow_hpp */
