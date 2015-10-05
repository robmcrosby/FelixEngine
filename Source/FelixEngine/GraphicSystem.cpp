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
    context->createResizableWindow();
  return context;
}

GraphicSystem* GraphicSystem::CreateMetalContext()
{
  return nullptr;
}
