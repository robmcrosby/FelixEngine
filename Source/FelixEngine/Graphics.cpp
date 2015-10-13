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
  // TODO: Release Resources when Resource counting is implemented.
}

bool Graphics::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (Component::setToXml(node) && mGraphicSystem)
  {
    success = true;
    for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
    {
      GraphicResource *resource = mGraphicSystem->getResource((*itr)->element(), (*itr)->attribute("name"));
      success &= resource && resource->setToXml(*itr);
      addResource(resource);
    }
  }
  return success;
}

bool Graphics::init()
{
  bool success = Component::init();
  return success;
}

void Graphics::addResource(fx::GraphicResource *resource)
{
  // TODO: Retain Resources when Resource counting is implemented.
  if (resource)
    mResources.push_back(resource);
}
