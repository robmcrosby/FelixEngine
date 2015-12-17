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


using namespace fx;
using namespace std;


Projection::Projection(Scene *scene): Component("Projection", scene), mType(PROJ_ORTHO),
mAspect(ASPECT_NONE), mRenderSlots(0), mLock(0), mDisparity(0), mZeroDistance(1.0f), mSwapAspect(0)
{
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
    if (node->hasAttribute("disparity"))
      setDisparity(node->attributeAsFloat("disparity"));
    if (node->hasAttribute("zero"))
      setZeroDistance(node->attributeAsFloat("zero"));
    if (node->hasSubNode("Volume"))
      success &= mVolume.setToXml(node->subNode("Volume"));
  }
  return success ? Component::setToXml(node) : false;
}

bool Projection::init()
{
  bool success = false;
  if (mParrent)
  {
    mRenderSlots = static_cast<RenderSlots*>(mParrent->getChildByType("RenderSlots"));
    success = mRenderSlots ? Component::init() : false;
  }
  return success;
}

void Projection::setType(const string &str)
{
  setType(GetProjectionType(str));
}

void Projection::setAspect(const string &str)
{
  setAspect(GetAspectType(str));
}

void Projection::update()
{
  if (mRenderSlots)
  {
    for (RenderSlots::iterator itr = mRenderSlots->begin(); itr != mRenderSlots->end(); ++itr)
    {
      Frame *frame = (*itr)->frame();
      vec2 size = frame ? vec2(frame->size()) : vec2(1.0f, 1.0f);
      (*itr)->uniforms().set("Projection", toMatrix4x4(size, (*itr)->stereoFlags()));
    }
  }
  Component::update();
}

mat4 Projection::toMatrix4x4(vec2 size, int flags) const
{
  lock();
  Volume v = mVolume;
  ASPECT_TYPE aspect = mAspect;
  PROJ_TYPE type = mType;
  float disparity = mDisparity/2.0f;
  float zeroDistance = mZeroDistance;
  unlock();
  
  if (mSwapAspect)
    size = size.yx();
  
  if ((flags == STEREO_LEFT || flags == STEREO_RIGHT) && type == PROJ_FRUSTUM)
  {
    float shift = disparity * (v.near/zeroDistance);
    if (flags == STEREO_LEFT)
    {
      v.left += shift;
      v.right += shift;
    }
    else
    {
      v.left -= shift;
      v.right -= shift;
      disparity = -disparity;
    }
  }
  else
    disparity = 0.0f;
  
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
  return mat4::Frustum(v.left, v.right, v.bottom, v.top, v.near, v.far) * mat4::Trans3d(vec3(disparity, 0.0f, 0.0f));
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
