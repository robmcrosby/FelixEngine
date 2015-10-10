//
//  Graphics.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Graphics.h"
#include "FelixEngine.h"
#include "GraphicSystem.h"

using namespace std;
using namespace fx;


DEFINE_COMPONENT_ID(Graphics);

Graphics::Graphics(Object *obj): Component("Graphics", obj)
{
  mGraphicSystem = dynamic_cast<GraphicSystem*>(FelixEngine::GetSystem(SYSTEM_GRAPHICS));
}

Graphics::~Graphics()
{
}

bool Graphics::setToXml(const XMLTree::Node *node)
{
  return Component::setToXml(node);
}

bool Graphics::init()
{
  bool success = Component::init();
  return success;
}
