//
//  GraphicResources.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "MeshLoader.h"


using namespace fx;
using namespace std;

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
    success = MeshLoader::LoadMeshFromXML(mBufferMap, node);
    if (success)
      setToReload();
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



Material::Material(GraphicSystem *system): mSystem(system)
{
}

Material::~Material()
{
}

bool Material::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    
  }
  return success;
}



View::View(GraphicSystem *system): mSystem(system)
{
}

View::~View()
{
}

bool View::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    
  }
  return success;
}
