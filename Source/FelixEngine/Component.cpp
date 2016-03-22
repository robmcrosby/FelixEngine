//
//  Component.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Component.h"

#include "Resources.h"
#include "RenderSlots.h"

#include "Projection.h"
#include "View.h"
#include "TrackingView.h"
#include "OrbitView.h"
#include "GyroView.h"
#include "Transform.h"
#include "Material.h"

#include "Model.h"
#include "Camera.h"

#include "UIWidget.h"
#include "UIScreen.h"
#include "UIText.h"


using namespace fx;
using namespace std;

DEFINE_COMPONENT_ID(Resources)
DEFINE_COMPONENT_ID(RenderSlots)

DEFINE_COMPONENT_ID(Transform)
DEFINE_COMPONENT_ID(View)
DEFINE_COMPONENT_ID(TrackingView)
DEFINE_COMPONENT_ID(OrbitView)
DEFINE_COMPONENT_ID(GyroView)
DEFINE_COMPONENT_ID(Projection)

DEFINE_COMPONENT_ID(Model)
DEFINE_COMPONENT_ID(Camera)
DEFINE_COMPONENT_ID(Material)

DEFINE_COMPONENT_ID(UIWidget)
DEFINE_COMPONENT_ID(UIScreen)
DEFINE_COMPONENT_ID(UIText)


Component::Component(Scene *scene): mParrent(0), mScene(scene)
{
  setEventFlags(EVENT_NONE);
}

Component::~Component()
{
  if (mParrent)
    mParrent->removeChild(this);
  clearChildren();
}

void Component::setToXml(const XMLTree::Node &node)
{
  if (node.hasAttribute("name"))
    setName(node.attribute("name"));
}

bool Component::init()
{
  bool success = true;
  for (iterator itr = begin(); itr != end(); ++itr)
    success &= (*itr)->init();
  return success;
}

void Component::update()
{
  for (iterator itr = begin(); itr != end(); ++itr)
    (*itr)->update();
}

void Component::addChildren(const XMLTree::Node &node)
{
  for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
    addChild(**itr);
}

void Component::addChild(const XMLTree::Node &node)
{
  Component *child = Create(node, mScene);
  if (child)
    addChild(child);
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

void Component::clearChildren()
{
  std::list<Component*> children = mChildren;
  mChildren.clear();
  
  for (iterator itr = children.begin(); itr != children.end(); ++itr)
    delete *itr;
}

Component* Component::getChild(const std::string &name) const
{
  for (const_iterator itr = begin(); itr != end(); ++itr)
  {
    if ((*itr)->name() == name)
      return *itr;
  }
  return nullptr;
}

Component* Component::getSibling(const std::string &name) const
{
  return mParrent ? mParrent->getChild(name) : nullptr;
}

map<string, Component::ComponentId*>& Component::GetComponentIdMap()
{
  static map<string, ComponentId*> compIdMap;
  return compIdMap;
}

Component* Component::Create(const std::string &type, Scene *scene)
{
  if (GetComponentIdMap().count(type))
    return GetComponentIdMap().at(type)->create(scene);
  return nullptr;
}

Component* Component::Create(const XMLTree::Node &node, Scene *scene)
{
  Component *comp = Create(node.element(), scene);
  if (comp)
    comp->setToXml(node);
  return comp;
}
