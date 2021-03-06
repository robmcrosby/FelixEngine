//
//  System.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "System.h"
#include "TaskingSystem.h"
#include "GCDTaskingSystem.h"
#include "SDLEventSystem.h"
#include "GLGraphicSystem.h"
//#include "MTLGraphicSystem.h"
#include "IOSMotionSystem.h"
#include "UbiquityFileSystem.h"


using namespace fx;
using namespace std;

DEFINE_SYSTEM_ID(TaskingSystem)
DEFINE_SYSTEM_ID(GCDTaskingSystem)
DEFINE_SYSTEM_ID(SDLEventSystem)
DEFINE_SYSTEM_ID(GLGraphicSystem)
//DEFINE_SYSTEM_ID(MTLGraphicSystem)
DEFINE_SYSTEM_ID(IOSMotionSystem)
DEFINE_SYSTEM_ID(UbiquityFileSystem)

TaskingSystem* TaskingSystem::sInstance = nullptr;
EventSystem* EventSystem::sInstance = nullptr;


System::System(SYSTEM_TYPE type): mType(type), mSDLInitFlags(0)
{
}

System::~System()
{
}

System* System::Create(const std::string &name)
{
  if (GetSystemIdMap().count(name))
    return GetSystemIdMap().at(name)->create();
  
  cerr << "Unknown System: " << name << endl;
  return nullptr;
}

map<string, System::SystemId*>& System::GetSystemIdMap()
{
  static map<string, System::SystemId*> systems;
  return systems;
}
