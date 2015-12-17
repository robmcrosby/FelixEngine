//
//  Component.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Component.h"

#include "RenderSlots.h"
#include "Projection.h"
#include "View.h"
#include "TrackingView.h"
#include "OrbitView.h"
#include "GyroView.h"
#include "Transform.h"

#include "Model.h"
#include "Camera.h"
#include "UIScreen.h"
#include "UIWidget.h"


using namespace fx;
using namespace std;

DEFINE_COMPONENT_ID(RenderSlots)
DEFINE_COMPONENT_ID(Transform)
DEFINE_COMPONENT_ID(View)
DEFINE_COMPONENT_ID(TrackingView)
DEFINE_COMPONENT_ID(OrbitView)
DEFINE_COMPONENT_ID(GyroView)
DEFINE_COMPONENT_ID(Projection)

DEFINE_COMPONENT_ID(Model)
DEFINE_COMPONENT_ID(Camera)
DEFINE_COMPONENT_ID(UIScreen)
DEFINE_COMPONENT_ID(UIWidget)


Component::Component(const std::string &type, Scene *scene): mType(type), mParrent(0), mScene(scene)
{
  setEventFlags(EVENT_NONE);
}

Component::~Component()
{
  if (mParrent)
    mParrent->removeChild(this);
  clearChildren();
}

bool Component::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    if (node->hasAttribute("name"))
      setName(node->attribute("name"));
  }
  return success;
}

bool Component::init()
{
  bool success = true;
  for (iterator itr = begin(); itr != end(); ++itr)
    success &= (*itr)->init();
  return success;
}

bool Component::addChildren(const XMLTree::Node &node)
{
  bool success = true;
  for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
  {
    Component *child = Create((*itr)->element(), mScene);
    if (child)
    {
      success &= child->setToXml(*itr);
      addChild(child);
    }
  }
  return success;
}

void Component::addChild(Component *child)
{
  if (child && child != this)
  {
    if (child->parrent())
      child->parrent()->removeChild(child);
    child->mParrent = this;
    mChildren.push_back(child);
  }
}

void Component::removeChild(Component *child)
{
  if (child && child->mParrent == this)
  {
    mChildren.remove(child);
    child->mParrent = nullptr;
  }
}

Component::iterator Component::removeChild(iterator itr)
{
  if (*itr && (*itr)->mParrent == this)
  {
    itr = mChildren.erase(itr);
    (*itr)->mParrent = nullptr;
  }
  return itr;
}

void Component::deleteChild(Component *child)
{
  if (child && child->mParrent == this)
  {
    mChildren.remove(child);
    delete child;
  }
}

Component::iterator Component::deleteChild(iterator itr)
{
  if (*itr && (*itr)->mParrent == this)
  {
    Component *child = *itr;
    itr = mChildren.erase(itr);
    delete child;
  }
  return itr;
}

void Component::clearChildren()
{
  std::list<Component*> children = mChildren;
  for (iterator itr = children.begin(); itr != children.end(); ++itr)
    delete *itr;
  mChildren.clear();
}

Component* Component::getChildByName(const std::string &name)
{
  for (iterator itr = begin(); itr != end(); ++itr)
  {
    if ((*itr)->name() == name)
      return *itr;
  }
  return nullptr;
}
Component* Component::getChildByType(const std::string &type)
{
  for (iterator itr = begin(); itr != end(); ++itr)
  {
    if ((*itr)->type() == type)
      return *itr;
  }
  return nullptr;
}


map<string, Component::ComponentId*>& Component::GetComponentIdMap()
{
  static map<string, ComponentId*> compIdMap;
  return compIdMap;
}
