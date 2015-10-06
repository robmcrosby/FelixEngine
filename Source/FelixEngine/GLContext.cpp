//
//  OpenGLContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLContext.h"
#include "GLWindow.h"

using namespace fx;
using namespace std;

DEFINE_SYSTEM_ID(GLContext)

GLContext::GLContext(): mSDL_GLContext(0)
{
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
}

GLContext::~GLContext()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    delete itr->second;
}

Window* GLContext::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
    mWindows[name] = new GLWindow(this);
  return mWindows[name];
}

void GLContext::setVersion(int major, int minor)
{
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
}

bool GLContext::setToXml(const XMLTree::Node &node)
{
  bool success = setVersion(node.subNode("Version"));
  
  if (success)
    addWindows(node.subNode("Windows"));
  
  return success;
}

bool GLContext::setVersion(const XMLTree::Node *node)
{
  if (node && node->hasAttribute("major") && node->hasAttribute("minor"))
  {
    setVersion(node->attributeAsInt("major"), node->attributeAsInt("minor"));
    return true;
  }
  
  cerr << "Warning: Unable to determine OpenGL Version from settings, using 2.1 for now." << endl;
  setVersion(2, 1);
  return false;
}

void GLContext::update()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
  {
    itr->second->setActive();
    
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    itr->second->swapBuffers();
  }
}
