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
  setEventFlags(EVENT_UPDATE);
  mTaskSlots.push_back(TaskSlot());
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
  if (node && node->hasAttribute("name"))
  {
    Window *window = getWindow(node->attribute("name"));
    success = window && window->setToXml(*node);
  }
  return success;
}

Resource* GraphicSystem::getResource(const std::string &type, const std::string &name)
{
  Resource *resource = nullptr;
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
  }
  return resource;
}

void GraphicSystem::handle(const fx::Event &event)
{
  if (event == EVENT_UPDATE)
    update();
}

void GraphicSystem::update()
{
  if (mUpdateMutex.tryLock())
  {
    notify(Event(EVENT_RENDER, DISPATCH_SERIAL));
    
    mTaskCollection.dump(mTaskBuffer);
    sort(mTaskBuffer.begin(), mTaskBuffer.end());
    loadTaskSlots();
    
    mUpdateMutex.unlock();
  }
}

void GraphicSystem::clearTaskSlots()
{
  for (TaskSlots::iterator itr = mTaskSlots.begin(); itr != mTaskSlots.end(); ++itr)
    itr->clear();
}

void GraphicSystem::loadTaskSlots()
{
  mTaskSlotsMutex.lock();
  clearTaskSlots();
  for (TaskBuffer::iterator itr = mTaskBuffer.begin(); itr != mTaskBuffer.end(); ++itr)
  {
    if (itr->isViewTask())
      mTaskSlots.at(0).push_back(*itr);
    else
    {
      if (itr->viewIndex >= (int)mTaskSlots.size())
        mTaskSlots.resize(itr->viewIndex+1);
      mTaskSlots.at(itr->viewIndex).push_back(*itr);
    }
  }
  mTaskSlotsMutex.unlock();
  mTaskBuffer.clear();
}
