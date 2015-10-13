//
//  GLWindow.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLWindow.h"
#include "GLGraphicSystem.h"
#include "GLFrame.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLWindow::GLWindow(GLGraphicSystem *system, const std::string &name): mName(name), mSDLWindow(0), mGLSystem(system)
{
  mGLFrame = static_cast<GLFrame*>(system->getFrame(name));
  mGLFrame->setToWindow(this);
}

GLWindow::~GLWindow()
{
  if (mSDLWindow)
    SDL_DestroyWindow(mSDLWindow);
}

bool GLWindow::load()
{
  if (!mLoaded)
  {
    // Get and check the SDL Window
    mSDLWindow = SDL_CreateWindow(mTitle.c_str(),
                                  mPosition.x, mPosition.y,
                                  mSize.w, mSize.h,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!mSDLWindow)
    {
      cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
      return false;
    }
    
    // Check if the OpenGL Context has been created already.
    if (!mGLSystem->getContext())
    {
      // Create and check the OpenGL Context
      SDL_GLContext context = SDL_GL_CreateContext(mSDLWindow);
      if (!context)
      {
        cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << endl;
        return false;
      }
      mGLSystem->setContext(context);
      
      // Set to Use V-Sync
      if (SDL_GL_SetSwapInterval(1) < 0)
        cerr << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << endl;
    }
    mLoaded = true;
  }
  return mLoaded;
}

void GLWindow::reload()
{
  GraphicResource::reload();
  if (mGLSystem)
    mGLSystem->loadOnNextUpdate();
}

void GLWindow::setTitle(const std::string &title)
{
  mTitle = title;
  if (mSDLWindow)
    SDL_SetWindowTitle(mSDLWindow, title.c_str());
}

void GLWindow::setPosition(const ivec2 &pos)
{
  mPosition = pos;
  if (mSDLWindow)
    SDL_SetWindowPosition(mSDLWindow, pos.x, pos.y);
}

void GLWindow::setSize(const ivec2 &size)
{
  mSize = size;
  if (mSDLWindow)
    SDL_SetWindowSize(mSDLWindow, size.w, size.h);
}

void GLWindow::setActive()
{
  SDL_GL_MakeCurrent(mSDLWindow, mGLSystem->getContext());
}

void GLWindow::swapBuffers()
{
  if (mSDLWindow)
    SDL_GL_SwapWindow(mSDLWindow);
}
