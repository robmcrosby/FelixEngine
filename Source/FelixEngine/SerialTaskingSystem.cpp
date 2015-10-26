//
//  SerialTaskingSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "SerialTaskingSystem.h"


using namespace fx;
using namespace std;

DEFINE_SYSTEM_ID(SerialTaskingSystem)


SerialTaskingSystem::SerialTaskingSystem()
{
  cout << "Created SerialTaskingSystem" << endl;
}

SerialTaskingSystem::~SerialTaskingSystem()
{
}

bool SerialTaskingSystem::setToXml(const XMLTree::Node *node)
{
  return true;
}

bool SerialTaskingSystem::init()
{
  return true;
}

void SerialTaskingSystem::update()
{
}
