//
//  RenderSlots.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "RenderSlots.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(RenderSlots);

RenderSlots::RenderSlots(Object *obj): Component("RenderSlots", obj)
{
}

RenderSlots::~RenderSlots()
{
}

bool RenderSlots::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool RenderSlots::init()
{
  return Component::init();
}
