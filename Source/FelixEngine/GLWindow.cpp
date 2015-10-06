//
//  GLWindow.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLWindow.h"
#include "GLContext.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLWindow::GLWindow(GLContext *context): mSDLWindow(0), mGLContext(context)
{
}

GLWindow::~GLWindow()
{
  if (mSDLWindow)
    SDL_DestroyWindow(mSDLWindow);
}

bool GLWindow::loadResizable(const std::string &title, ivec2 size)
{
  return loadResizable(title, size, ivec2(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED));
}

bool GLWindow::loadResizable(const std::string &title, ivec2 size, ivec2 pos)
{
  mSDLWindow = SDL_CreateWindow(title.c_str(), pos.x, pos.y, size.w, size.h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!mSDLWindow)
  {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return false;
  }
  
  // Check if the OpenGL Context has been created already.
  if (!mGLContext->getSDL_GLContext())
  {
    // Create and check the OpenGL Context
    SDL_GLContext context = SDL_GL_CreateContext(mSDLWindow);
    if (!context)
    {
      cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << endl;
      return false;
    }
    mGLContext->setSDL_GLContext(context);
    
    // Set to Use V-Sync
    if (SDL_GL_SetSwapInterval( 1 ) < 0)
      cerr << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << endl;
  }
  
  return true;
}

void GLWindow::setActive()
{
}

void GLWindow::swapBuffers()
{
  if (mSDLWindow)
    SDL_GL_SwapWindow(mSDLWindow);
}
