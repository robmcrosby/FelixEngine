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

//void Frame::setRefrenceFrame(const std::string &name)
//{
//  GraphicSystem *sys = FelixEngine::GetGraphicSystem();
//  if (sys && name != "")
//    setRefrenceFrame(sys->getFrame(name));
//  else
//    setRefrenceFrame(nullptr);
//}
