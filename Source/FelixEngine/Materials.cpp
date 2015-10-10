//
//  Materials.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Materials.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Materials);

Materials::Materials(Object *obj): Component("Materials", obj)
{
}

Materials::~Materials()
{
}

bool Materials::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Materials::init()
{
  return Component::init();
}
