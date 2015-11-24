//
//  Projection.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Projection.h"
#include "FelixEngine.h"
#include "GraphicSystem.h"
#include "RenderSlots.h"
#include "Object.h"


using namespace fx;
using namespace std;


Projection::Projection(Object *obj): Component("Projection", obj), mType(PROJ_ORTHO),
mAspect(ASPECT_NONE), mRenderSlots(0), mLock(0)
{
  mUpdateDelegate = UpdateDelegate::Create<Projection, &Projection::update>(this);
}

Projection::~Projection()
{
}

bool Projection::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    if (node->hasAttribute("type"))
      setType(node->attribute("type"));
    if (node->hasAttribute("aspect"))
      setAspect(node->attribute("aspect"));
    if (node->hasSubNode("Volume"))
      success &= mVolume.setToXml(node->subNode("Volume"));
  }
  return success ? Component::setToXml(node) : false;
}

bool Projection::init()
{
  mRenderSlots = static_cast<RenderSlots*>(mObject->getComponentByType("RenderSlots"));
  return mRenderSlots ? Component::init() : false;
}

void Projection::setType(const string &str)
{
  setType(GetProjectionType(str));
}

void Projection::setAspect(const string &str)
{
  setAspect(GetAspectType(str));
}

void Projection::update(void *)
{
  if (mRenderSlots)
  {
    for (RenderSlots::iterator itr = mRenderSlots->begin(); itr != mRenderSlots->end(); ++itr)
    {
      Frame *frame = (*itr)->frame();
      vec2 size = frame ? vec2(frame->size()) : vec2(1.0f, 1.0f);
      (*itr)->uniforms().set("Projection", toMatrix4x4(size));
    }
  }
}

mat4 Projection::toMatrix4x4(vec2 size) const
{
  lock();
  Volume v = mVolume;
  ASPECT_TYPE aspect = mAspect;
  PROJ_TYPE type = mType;
  unlock();
  
  if (aspect == ASPECT_WIDTH || (aspect == ASPECT_AUTO && size.w < size.h))
  {
    float aspectRatio = size.h/size.w;
    v.top *= aspectRatio;
    v.bottom *= aspectRatio;
  }
  else if (aspect == ASPECT_HEIGHT || (aspect == ASPECT_AUTO && size.h < size.w))
  {
    float aspectRatio = size.w/size.h;
    v.left *= aspectRatio;
    v.right *= aspectRatio;
  }
  
  if (type == PROJ_ORTHO)
    return mat4::Ortho(v.left, v.right, v.bottom, v.top, v.near, v.far);
  return mat4::Frustum(v.left, v.right, v.bottom, v.top, v.near, v.far);
}

PROJ_TYPE Projection::GetProjectionType(const std::string &str)
{
  if (str == "ortho")
    return PROJ_ORTHO;
  return PROJ_FRUSTUM;
}

ASPECT_TYPE Projection::GetAspectType(const std::string &str)
{
  if (str == "auto")
    return ASPECT_AUTO;
  if (str == "width")
    return ASPECT_WIDTH;
  if (str == "height")
    return ASPECT_HEIGHT;
  return ASPECT_NONE;
}



bool Volume::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    near   = node->attributeAsFloat("near");
    far    = node->attributeAsFloat("far");
    left   = node->attributeAsFloat("left");
    right  = node->attributeAsFloat("right");
    top    = node->attributeAsFloat("top");
    bottom = node->attributeAsFloat("bottom");
  }
  return success;
}
