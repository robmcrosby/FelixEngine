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


using namespace fx;
using namespace std;

DEFINE_SYSTEM_ID(TaskingSystem)
DEFINE_SYSTEM_ID(GCDTaskingSystem)


System::System(SYSTEM_TYPE type): mType(type), mSDLInitFlags(0)
{
  mUpdateDelegate = UpdateDelegate::Create<System, &System::update>(this);
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
