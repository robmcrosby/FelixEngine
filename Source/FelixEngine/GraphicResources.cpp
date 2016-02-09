//
//  GraphicResources.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "GraphicSystem.h"
#include "FelixEngine.h"
#include "MeshLoader.h"
#include "FileSystem.h"


using namespace fx;
using namespace std;



bool Window::setToXml(const XMLTree::Node &node)
{
  bool success = true;
  if (node.hasAttribute("title"))
    setTitle(node.attribute("title"));
  if (node.hasSubNode("position"))
    setPosition(node.subContents("position"));
  if (node.hasSubNode("size"))
    setSize(node.subContents("size"));
  return success;
}



//bool Frame::setToXml(const XMLTree::Node &node)
//{
//  bool success = true;
//  
//  // Set the size
//  setSize(ivec2(0, 0));
//  setScale(vec2(1.0f, 1.0f));
//  if (node.hasAttribute("width") && node.hasAttribute("height"))
//  {
//    int width  = node.attributeAsInt("width");
//    int height = node.attributeAsInt("height");
//    setSize(ivec2(width, height));
//  }
//  else
//  {
//    setRefrenceFrame(node.hasAttribute("frame") ? node.attribute("frame") : MAIN_WINDOW);
//    if (node.hasAttribute("scale"))
//      setScale(node.attribute("scale"));
//  }
//  return success;
//}

void Frame::setRefrenceFrame(const std::string &name)
{
  GraphicSystem *sys = FelixEngine::GetGraphicSystem();
  if (sys && name != "")
    setRefrenceFrame(sys->getFrame(name));
  else
    setRefrenceFrame(nullptr);
}



SHADER_PART Shader::ParseShaderPart(const std::string &partStr)
{
  if (partStr == "Vertex")
    return SHADER_VERTEX;
  if (partStr == "Fragment")
    return SHADER_FRAGMENT;
  return SHADER_COUNT;
}
