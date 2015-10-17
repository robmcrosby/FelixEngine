//
//  GpuSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicSystem.h"
#include "UniformMap.h"


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

//Resource* GraphicSystem::getResource(const std::string &type, const std::string &name)
//{
//  Resource *resource = nullptr;
//  if (name == "")
//    cerr << "Error: Blank name for Graphic Resource: " << type << endl;
//  else
//  {
//    if (type == "Window")
//      resource = getWindow(name);
//    else if (type == "Frame")
//      resource = getFrame(name);
//    else if (type == "Mesh")
//      resource = getMesh(name);
//    else if (type == "Shader")
//      resource = getShader(name);
//    else if (type == "Texture")
//      resource = getTexture(name);
//    else
//      cerr << "Error: Unknown Graphic Resource: " << type << endl;
//  }
//  return resource;
//}
