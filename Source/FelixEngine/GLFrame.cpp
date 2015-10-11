//
//  GLFrame.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLFrame.h"
#include "GLGraphicSystem.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLFrame::GLFrame(GLGraphicSystem *system): mGLSystem(system)
{
}

GLFrame::~GLFrame()
{
}

bool GLFrame::load()
{
  return true;
}

void GLFrame::addBuffer(BUFFER_TYPE type, const std::string &name, const Sampler &sampler)
{
}

void GLFrame::setSize(const ivec2 &size)
{
}
