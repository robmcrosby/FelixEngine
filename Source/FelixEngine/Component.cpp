//
//  Component.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Component.h"
#include "RenderSlots.h"
#include "ViewSlots.h"


using namespace fx;
using namespace std;

DEFINE_COMPONENT_ID(RenderSlots);
DEFINE_COMPONENT_ID(ViewSlots);


map<string, Component::ComponentId*>& Component::GetComponentIdMap()
{
  static map<string, ComponentId*> compIdMap;
  return compIdMap;
}
