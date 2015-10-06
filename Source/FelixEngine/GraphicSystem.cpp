//
//  GpuSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GraphicSystem.h"
#include "GLContext.h"


using namespace fx;
using namespace std;

GraphicSystem* GraphicSystem::CreateOpenGLContext()
{
  GLContext *context = new GLContext();
  if (context)
  {
    Window *window = context->getWindow("MainWindow");
    window->loadResizable("Title", ivec2(640, 400), ivec2(100, 100));
  }
  return context;
}

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
    success = window && window->setToXml(*node);
  }
  return success;
}




bool Window::setToXml(const XMLTree::Node &node)
{
  bool success = false;
  if (node.hasAttribute("title") && node.hasSubNode("size"))
  {
    if (node.hasSubNode("position"))
      success = loadResizable(node.attribute("title"), node.subContents("size"), node.subContents("position"));
    else
      success = loadResizable(node.attribute("title"), node.subContents("size"));
  }
  
  return success;
}
