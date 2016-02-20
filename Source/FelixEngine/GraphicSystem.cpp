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
  mUpdateTaskBufferDelgate = TaskDelegate::Create<GraphicSystem, &GraphicSystem::updateTaskBuffer>(this);
}

GraphicSystem::~GraphicSystem()
{
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

void GraphicSystem::updateTasks()
{
  // Launch a task start getting new tasks.
  mTaskBufferMutex.lock();
  TaskingSystem::Instance()->dispatch(mUpdateTaskBufferDelgate);
  
  // Clear the previous Tasks
  mPreTasks.clear();
  mPostTasks.clear();
  for (vector<TaskPass>::iterator itr = mTaskPasses.begin(); itr != mTaskPasses.end(); ++itr)
    itr->clear();
  
  // Organize the new Tasks
  for (TaskPass::iterator itr = mTaskBuffer.begin(); itr != mTaskBuffer.end(); ++itr)
  {
    switch (itr->type)
    {
      case GRAPHIC_TASK_UPLOAD:
        mPreTasks.push_back(*itr);
        break;
      case GRAPHIC_TASK_UNLOAD:
      case GRAPHIC_TASK_DOWNLOAD:
        mPostTasks.push_back(*itr);
        break;
      case GRAPHIC_TASK_PASS:
        addTaskToPass(*itr, 0);
        break;
      case GRAPHIC_TASK_DRAW:
      case GRAPHIC_TASK_COMPUTE:
        addTaskToPass(*itr, itr->pass);
        break;
      case GRAPHIC_TASK_EMPTY:
        break;
    }
  }
  mTaskBuffer.clear();
  mTaskBufferMutex.unlock();
}

void GraphicSystem::updateTaskBuffer(void*)
{
  notify(Event(EVENT_APP_RENDER, DISPATCH_SERIAL));
  
  mTaskBufferMutex.lock();
  mTaskBuffer.clear();
  
  mTaskCollection.dump(mTaskBuffer);
  sort(mTaskBuffer.begin(), mTaskBuffer.end());
  mTaskBufferMutex.unlock();
}
