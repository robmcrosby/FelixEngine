//
//  Projection.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Projection.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Projection);

Projection::Projection(Object *obj): Component("Projection", obj)
{
}

Projection::~Projection()
{
}

bool Projection::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Projection::init()
{
  return Component::init();
}
