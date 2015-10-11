//
//  Transform.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/9/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Transform.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Transform);

Transform::Transform(Object *obj): Component("Transform", obj)
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
  return Component::init();
}