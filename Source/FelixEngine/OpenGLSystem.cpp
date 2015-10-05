//
//  OpenGLSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "OpenGLSystem.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


OpenGLSystem::OpenGLSystem()
{
  
}

OpenGLSystem::~OpenGLSystem()
{
  
}

GPUWindow* OpenGLSystem::createResizableWindow()
{
  GLWindow *window = new GLWindow();
  
  window->mSdlWindow = SDL_CreateWindow("Felix Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  if (window->mSdlWindow == NULL)
  {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    delete window;
    return nullptr;
  }
  return window;
}

void OpenGLSystem::update()
{
  
}
