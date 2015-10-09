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

Transform::Transform(): Component("Transform")
{
  
}

Transform::~Transform()
{
  
}

bool Transform::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}
