//
//  GpuSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GpuSystem.h"
#include "OpenGLSystem.h"


using namespace fx;
using namespace std;

GpuSystem* GpuSystem::CreateOpenGLSystem()
{
  OpenGLSystem *system = new OpenGLSystem();
  if (system)
    system->createResizableWindow();
  return system;
}

GpuSystem* GpuSystem::CreateMetalSystem()
{
  return nullptr;
}
