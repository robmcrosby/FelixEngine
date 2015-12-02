//
//  Component.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Component.h"
#include "Object.h"

#include "RenderSlots.h"
#include "Projection.h"
#include "View.h"
#include "TrackingView.h"
#include "OrbitView.h"
#include "GyroView.h"
#include "Transform.h"


using namespace fx;
using namespace std;

DEFINE_COMPONENT_ID(RenderSlots);
DEFINE_COMPONENT_ID(Transform);
DEFINE_COMPONENT_ID(View);
DEFINE_COMPONENT_ID(TrackingView);
DEFINE_COMPONENT_ID(OrbitView);
DEFINE_COMPONENT_ID(GyroView);
DEFINE_COMPONENT_ID(Projection);


Component::Component(const std::string &type, Object *obj): mType(type), mObject(obj)
{
  setEventFlags(EVENT_NONE);
  mUpdateDelegate = UpdateDelegate::Create<Component, &Component::update>(this);
  
  if (mObject)
    mObject->addComponent(this);
}

Component::~Component()
{
  if (mObject)
    mObject->removeComponent(this);
}

map<string, Component::ComponentId*>& Component::GetComponentIdMap()
{
  static map<string, ComponentId*> compIdMap;
  return compIdMap;
}
