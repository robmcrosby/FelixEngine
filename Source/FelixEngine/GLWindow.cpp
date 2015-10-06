//
//  GLWindow.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLWindow.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLWindow::GLWindow(): mSDLWindow(0)
{
}

GLWindow::~GLWindow()
{
  if (mSDLWindow)
    SDL_DestroyWindow(mSDLWindow);
}

bool GLWindow::loadResizable(const std::string &title, ivec2 pos, ivec2 size)
{
  //mSDLWindow = SDL_CreateWindow("Felix Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  mSDLWindow = SDL_CreateWindow(title.c_str(), pos.x, pos.y, size.w, size.h, SDL_WINDOW_SHOWN);
  if (!mSDLWindow)
  {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return false;
  }
  return true;
}

void GLWindow::swapBuffers()
{
  if (mSDLWindow)
    SDL_UpdateWindowSurface(mSDLWindow);
}
