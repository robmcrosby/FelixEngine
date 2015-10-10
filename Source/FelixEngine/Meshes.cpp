//
//  Meshes.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Meshes.h"


using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Meshes);

Meshes::Meshes(): Component("Meshes")
{
}

Meshes::~Meshes()
{
}

bool Meshes::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Meshes::init()
{
  return Component::init();
}
