//
//  ViewSlots.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "ViewSlots.h"
#include "Scene.h"


using namespace fx;
using namespace std;


ViewSlots::ViewSlots(Object *obj): Component("ViewSlots", obj)
{
  mScene = obj ? obj->getScene() : nullptr;
  mUpdateDelegate = UpdateDelegate::Create<ViewSlots, &ViewSlots::update>(this);
}

ViewSlots::~ViewSlots()
{
  
}

bool ViewSlots::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
      success &= addView(**itr);
  }
  return success;
}

bool ViewSlots::init()
{
  return Component::init();
}

void ViewSlots::update(void*)
{
}

bool ViewSlots::addView(const XMLTree::Node &node)
{
  bool success = false;
  if (mScene && node.hasAttribute("name"))
  {
    View *view = mScene->getView(node.attribute("name"));
    success = view->setToXml(node);
    mViews.push_back(view);
  }
  return success;
}
