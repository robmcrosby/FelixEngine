//
//  GCDTaskingSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GCDTaskingSystem.h"


using namespace fx;
using namespace std;

DEFINE_SYSTEM_ID(GCDTaskingSystem)


GCDTaskingSystem::GCDTaskingSystem()
{
  mUpdateDelegate = UpdateDelegate::Create<GCDTaskingSystem, &GCDTaskingSystem::update>(this);
}

GCDTaskingSystem::~GCDTaskingSystem()
{
}

bool GCDTaskingSystem::setToXml(const XMLTree::Node *node)
{
  return true;
}

bool GCDTaskingSystem::init()
{
  return true;
}

void GCDTaskingSystem::update(void*)
{
}
