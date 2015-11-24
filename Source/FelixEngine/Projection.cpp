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
mAspect(ASPECT_NONE), mFrame(0), mRenderSlots(0), mLock(0)
{
  updateMatrix();
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
    if (node->hasAttribute("frame"))
      setFrame(node->attribute("frame"));
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

void Projection::setFrame(const std::string &name)
{
  GraphicSystem *sys = FelixEngine::GetGraphicSystem();
  if (sys)
    setFrame(sys->getFrame(name));
}

void Projection::update(void *)
{
  updateMatrix();
  lock();
  if (mRenderSlots)
  {
    for (RenderSlots::iterator itr = mRenderSlots->begin(); itr != mRenderSlots->end(); ++itr)
      (*itr)->uniforms().set("Projection", mMatrix);
  }
  unlock();
}

void Projection::updateMatrix()
{
  lock();
  
  Volume v = mVolume;
  if (mFrame)
  {
    float width = (float)mFrame->size().w;
    float height = (float)mFrame->size().h;
    
    if (mAspect == ASPECT_WIDTH || (mAspect == ASPECT_AUTO && width < height))
    {
      float aspect = height/width;
      v.top *= aspect;
      v.bottom *= aspect;
    }
    else if (mAspect == ASPECT_HEIGHT || (mAspect == ASPECT_AUTO && height < width))
    {
      float aspect = width/height;
      v.left *= aspect;
      v.right *= aspect;
    }
  }
  
  if (mType == PROJ_ORTHO)
    mMatrix = mat4::Ortho(v.left, v.right, v.bottom, v.top, v.near, v.far);
  else if (mType == PROJ_FRUSTUM)
    mMatrix = mat4::Frustum(v.left, v.right, v.bottom, v.top, v.near, v.far);
  
  unlock();
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
