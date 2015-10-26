//
//  GraphicSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GraphicSystem_h
#define GraphicSystem_h

#include "System.h"
#include "GraphicResources.h"
#include "GraphicTask.h"


namespace fx
{
  class UniformMap;
  class InternalUniformMap;
  
  /**
   *
   */
  class GraphicSystem: public System
  {
  public:
    GraphicSystem(): System(SYSTEM_GRAPHICS) {mTaskSlots.push_back(TaskSlot());}
    virtual ~GraphicSystem() {}
    
    virtual Window* getWindow(const std::string &name) = 0;
    virtual Frame* getFrame(const std::string &name) = 0;
    virtual Shader* getShader(const std::string &name) = 0;
    virtual Mesh* getMesh(const std::string &name) = 0;
    virtual Texture* getTexture(const std::string &name) = 0;
    
    Resource* getResource(const std::string &type, const std::string &name);
    virtual InternalUniformMap* getInternalUniformMap(UniformMap *map) = 0;
    
    void addGraphicTask(const GraphicTask &task)
    {
      // TODO: make this work with multiple threads.
      mTaskList.push_back(task);
    }
    
  protected:
    typedef std::list<GraphicTask> TaskList;
    typedef std::vector<GraphicTask> TaskSlot;
    typedef std::vector<TaskSlot> TaskSlots;
    
    void clearTaskSlots()
    {
      for (TaskSlots::iterator itr = mTaskSlots.begin(); itr != mTaskSlots.end(); ++itr)
        itr->clear();
    }
    
    void loadTaskSlots()
    {
      mTaskList.sort();
      for (TaskList::iterator itr = mTaskList.begin(); itr != mTaskList.end(); ++itr)
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
      mTaskList.clear();
    }
    
    TaskList  mTaskList;
    TaskSlots mTaskSlots;
    
  protected:
    bool addWindows(const XMLTree::Node *node);
    bool addWindow(const XMLTree::Node *node);
  };
}


#endif /* GPU_System_h */
