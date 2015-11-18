//
//  View.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/17/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "View.h"
#include "Object.h"
#include "RenderSlots.h"


using namespace std;
using namespace fx;


View::View(Object *obj): Component("View", obj), mRenderSlots(0)
{
  mUpdateDelegate = UpdateDelegate::Create<View, &View::update>(this);
  
  mPosition = vec3(0.0f, 0.0f, 0.0f);
  mTarget = vec3(0.0f, 0.0f, -1.0f);
  mUp = vec3(0.0f, 1.0f, 0.0f);
}

View::~View()
{
}

bool View::setToXml(const XMLTree::Node *node)
{
  if (node)
  {
    if (node->hasSubNode("Position"))
      mPosition = node->subContents("Position");
    if (node->hasSubNode("Target"))
      mTarget = node->subContents("Target");
    if (node->hasSubNode("Up"))
      mUp = node->subContents("Up");
  }
  return Component::setToXml(node);
}

bool View::init()
{
  mRenderSlots = static_cast<RenderSlots*>(mObject->getComponentByType("RenderSlots"));
  updateMatrix();
  return mRenderSlots ? Component::init() : false;
}

void View::updateMatrix()
{
  mMatrix = mat4::LookAt(mPosition, mTarget, mUp);
}

void View::update(void*)
{
  updateMatrix();
  if (mRenderSlots)
    mRenderSlots->localUniforms()["View"] = mMatrix;
}
