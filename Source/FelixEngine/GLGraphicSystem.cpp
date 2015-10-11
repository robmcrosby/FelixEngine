//
//  GLGraphicSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLGraphicSystem.h"

#include "GLWindow.h"
#include "GLFrame.h"
#include "GLShader.h"
#include "GLMesh.h"
#include "GLTexture.h"


using namespace fx;
using namespace std;


DEFINE_SYSTEM_ID(GLGraphicSystem)


GLGraphicSystem::GLGraphicSystem(): mContext(0)
{
  mInitFlags |= SDL_INIT_VIDEO;
}

GLGraphicSystem::~GLGraphicSystem()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    delete itr->second;
}

Window* GLGraphicSystem::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
    mWindows[name] = new GLWindow(this);
  return mWindows[name];
}

Frame* GLGraphicSystem::getFrame(const std::string &name)
{
  if (!mFrames.count(name))
    mFrames[name] = new GLFrame(this);
  return mFrames[name];
}

Shader* GLGraphicSystem::getShader(const std::string &name)
{
  if (!mShaders.count(name))
    mShaders[name] = new GLShader(this);
  return mShaders[name];
}

Mesh* GLGraphicSystem::getMesh(const std::string &name)
{
  if (!mMeshes.count(name))
    mMeshes[name] = new GLMesh(this);
  return mMeshes[name];
}

Texture* GLGraphicSystem::getTexture(const std::string &name)
{
  if (!mTextures.count(name))
    mTextures[name] = new GLTexture(this);
  return mTextures[name];
}

void GLGraphicSystem::setVersion(int major, int minor)
{
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
}

bool GLGraphicSystem::setToXml(const XMLTree::Node *node)
{
  if (node)
  {
    setVersion(node->subNode("Version"));
    return addWindows(node->subNode("Windows"));
  }
  cerr << "Error: GLGraphicSystem passed a NULL node." << endl;
  return false;
}

bool GLGraphicSystem::init()
{
  bool success = true;
  for (map<std::string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    success &= itr->second->load();
  return success;
}

bool GLGraphicSystem::setVersion(const XMLTree::Node *node)
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

void GLGraphicSystem::update()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
  {
    itr->second->setActive();
    
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    itr->second->swapBuffers();
  }
}
