//
//  GLTexture.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLTexture.h"
#include "GLGraphicSystem.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLTexture::GLTexture(GLGraphicSystem *system): mGLSystem(system)
{
}

GLTexture::~GLTexture()
{
}

void GLTexture::reload()
{
  GraphicResource::reload();
  if (mGLSystem)
    mGLSystem->loadOnNextUpdate();
}

bool GLTexture::load()
{
  if (!mLoaded)
  {
    mLoaded = true;
  }
  return mLoaded;
}