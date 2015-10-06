//
//  System.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "System.h"


using namespace fx;
using namespace std;


System::System(SYSTEM_TYPE type): mType(type), mInitFlags(0)
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
