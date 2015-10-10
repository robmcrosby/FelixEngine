//
//  Views.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Views.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Views);

Views::Views(): Component("Views")
{
}

Views::~Views()
{
}

bool Views::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Views::init()
{
  return Component::init();
}
