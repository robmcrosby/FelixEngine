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
  mUpdateDelegate = UpdateDelegate::Create<Transform, &Transform::update>(this);
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
  updateMatrices();
  return mRenderSlots ? Component::init() : false;
}

void Transform::updateMatrices()
{
}

void Transform::update(void*)
{
  updateMatrices();
  if (mRenderSlots)
  {
    mRenderSlots->localUniforms()["Model"] = mModelMatrix;
    mRenderSlots->localUniforms()["Rotation"] = mRotationMatrix;
  }
}
