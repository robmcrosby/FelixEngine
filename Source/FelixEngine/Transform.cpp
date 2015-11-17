//
//  Transform.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/9/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Transform.h"
#include "Object.h"
#include "RenderSlots.h"


using namespace std;
using namespace fx;


Transform::Transform(Object *obj): Component("Transform", obj), mRenderSlots(0)
{
}

Transform::~Transform()
{
}

bool Transform::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Transform::init()
{
  mRenderSlots = static_cast<RenderSlots*>(mObject->getComponentByType("RenderSlots"));
  
  if (mRenderSlots)
    mRenderSlots->localUniforms()["Model"] = mat4::Scale(vec3(0.5, 0.5, 0.5));
  
  return mRenderSlots ? Component::init() : false;
}

void Transform::update()
{
}
