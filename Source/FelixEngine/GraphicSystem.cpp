//
//  GpuSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicSystem.h"


using namespace fx;
using namespace std;

bool GraphicSystem::addWindows(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    for (XMLTree::const_iterator itr = node->begin(); success && itr != node->end(); ++itr)
      success &= addWindow(*itr);
  }
  
  return success;
}

bool GraphicSystem::addWindow(const XMLTree::Node *node)
{
  bool success = false;
  if (node && node->hasAttribute("name"))
  {
    Window *window = getWindow(node->attribute("name"));
    success = window && window->setToXml(node);
  }
  return success;
}

Material* GraphicSystem::getMaterial(const std::string &name)
{
  if (!mMaterials.count(name))
    mMaterials[name] = new Material(this);
  return mMaterials.at(name);
}

View* GraphicSystem::getView(const std::string &name)
{
  if (!mViews.count(name))
    mViews[name] = new View(this);
  return mViews.at(name);
}

GraphicResource* GraphicSystem::getResource(const std::string &type, const std::string &name)
{
  GraphicResource *resource = nullptr;
  if (name == "")
    cerr << "Error: Blank name for Graphic Resource: " << type << endl;
  else
  {
    if (type == "Window")
      resource = getWindow(name);
    else if (type == "Frame")
      resource = getFrame(name);
    else if (type == "Mesh")
      resource = getMesh(name);
    else if (type == "Shader")
      resource = getShader(name);
    else if (type == "Texture")
      resource = getTexture(name);
    else if (type == "Material")
      resource = getMaterial(name);
    else if (type == "View")
      resource = getView(name);
    else
      cerr << "Error: Unknown Graphic Resource: " << type << endl;
  }
  return resource;
}

