//
//  GpuSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicSystem.h"
#include "GLContext.h"


using namespace fx;
using namespace std;

GraphicSystem* GraphicSystem::CreateOpenGLContext()
{
  GLContext *context = new GLContext();
  if (context)
  {
    Window *window = context->getWindow("MainWindow");
    window->loadResizable("Title", ivec2(640, 400), ivec2(100, 100));
  }
  return context;
}