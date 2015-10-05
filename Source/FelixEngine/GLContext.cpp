//
//  OpenGLContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLContext.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLContext::GLContext()
{
  
}

GLContext::~GLContext()
{
  
}

Window* GLContext::createResizableWindow()
{
  GLWindow *window = new GLWindow();
  
  window->mWindow = SDL_CreateWindow("Felix Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  if (window->mWindow == NULL)
  {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    delete window;
    return nullptr;
  }
  return window;
}

void GLContext::update()
{
  
}
