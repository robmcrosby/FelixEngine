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


GraphicSystem::GraphicSystem(): System(SYSTEM_GRAPHICS)
{
  setEventFlags(EVENT_APP_UPDATE);
  mPasses.push_back(Pass());
}

GraphicSystem::~GraphicSystem()
{
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
  if (node)
  {
    Window *window = node->hasSubNode("name") ? getWindow(node->attribute("name")) : getWindow(MAIN_WINDOW);
    success = window && window->setToXml(*node);
  }
  return success;
}

Resource* GraphicSystem::getResource(const std::string &type, const std::string &name)
{
  if (type == "Window")
    return name == "" ? getWindow(MAIN_WINDOW) : getWindow(name);
  if (type == "Frame")
    return name == "" ? getFrame(MAIN_WINDOW) : getFrame(name);
  if (type == "Mesh")
    return getMesh(name);
  if (type == "Shader")
    return getShader(name);
  if (type == "Texture")
    return getTexture(name);
  return nullptr;
}

void GraphicSystem::handle(const fx::Event &event)
{
  if (event == EVENT_APP_UPDATE)
    update();
}

void GraphicSystem::update()
{
  if (mUpdateMutex.tryLock())
  {
    notify(Event(EVENT_APP_RENDER, DISPATCH_SERIAL));
    
    mTaskCollection.dump(mTaskBuffer);
    sort(mTaskBuffer.begin(), mTaskBuffer.end());
    loadPasses();
    
    mUpdateMutex.unlock();
  }
}

void GraphicSystem::clearPasses()
{
  for (Passes::iterator itr = mPasses.begin(); itr != mPasses.end(); ++itr)
    itr->clear();
}

void GraphicSystem::loadPasses()
{
  mPassesMutex.lock();
  clearPasses();
  for (Pass::iterator itr = mTaskBuffer.begin(); itr != mTaskBuffer.end(); ++itr)
  {
    if (itr->isViewTask())
      mPasses.at(0).push_back(*itr);
    else
    {
      if (itr->pass >= (int)mPasses.size())
        mPasses.resize(itr->pass+1);
      mPasses.at(itr->pass).push_back(*itr);
    }
  }
  mPassesMutex.unlock();
  mTaskBuffer.clear();
}

int GraphicSystem::GetStereoFlags(const std::string &flags)
{
  if (flags == "left")
    return STEREO_LEFT;
  if (flags == "right")
    return STEREO_RIGHT;
  if (flags == "binary")
    return STEREO_BINARY;
  if (flags == "mono")
    return STEREO_MONO;
  return STEREO_ALL;
}
