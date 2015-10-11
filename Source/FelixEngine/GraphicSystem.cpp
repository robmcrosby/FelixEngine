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




bool Window::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    if (node->hasAttribute("title"))
      setTitle(node->attribute("title"));
    if (node->hasSubNode("position"))
      setPosition(node->subContents("position"));
    if (node->hasSubNode("size"))
      setSize(node->subContents("size"));
  }
  return success;
}



bool Frame::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    
  }
  return success;
}



bool Shader::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    
  }
  return success;
}



bool Mesh::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    
  }
  return success;
}

bool Texture::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    
  }
  return success;
}
