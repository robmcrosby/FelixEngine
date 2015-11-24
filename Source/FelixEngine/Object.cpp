//
//  Object.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Object.h"
#include "Model.h"
#include "Camera.h"


using namespace fx;
using namespace std;


DEFINE_OBJECT_ID(Model)
DEFINE_OBJECT_ID(Camera)


Object::Object(const string &type, Scene *scene): mType(type), mScene(scene)
{
  setEventFlags(EVENT_NONE);
}

Object::~Object()
{
  clearComponents();
}

bool Object::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    if (node->hasAttribute("name"))
      setName(node->attribute("name"));
    for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
      success &= (bool)Component::Create(*itr, this);
  }
  return success;
}

bool Object::init()
{
  bool success = true;
  for (iterator itr = begin(); itr != end(); ++itr)
    success &= (*itr)->init();
  return success;
}

map<string, Object::ObjectId*>& Object::GetObjectIdMap()
{
  static map<string, ObjectId*> objIdMap;
  return objIdMap;
}
