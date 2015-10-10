//
//  Shaders.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Shaders.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Shaders);

Shaders::Shaders(Object *obj): Component("Shaders", obj)
{
}

Shaders::~Shaders()
{
}

bool Shaders::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Shaders::init()
{
  return Component::init();
}
