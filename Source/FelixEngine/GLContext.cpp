//
//  OpenGLContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLContext.h"
#include "GLWindow.h"

using namespace fx;
using namespace std;


GLContext::GLContext()
{
  
}

GLContext::~GLContext()
{
  
}

Window* GLContext::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
    mWindows[name] = new GLWindow();
  return mWindows[name];
}

void GLContext::update()
{
  
}
