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
#include "TaskingSystem.h"
#include "GraphicTask.h"
#include "MultiVector.h"
#include "Mutex.h"


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
    GraphicSystem();
    virtual ~GraphicSystem();

    virtual void render() = 0;
    
    virtual Window* getWindow(const std::string &name) = 0;
    virtual Frame* getFrame(const std::string &name) = 0;
    virtual Shader* getShader(const std::string &name) = 0;
    virtual Mesh* getMesh(const std::string &name) = 0;
    virtual Texture* getTexture(const std::string &name) = 0;
    
    Resource* getResource(const std::string &type, const std::string &name);
    
    void addGraphicTask(const GraphicTask &task) {mTaskCollection.append(task);}
    void uploadBuffer(BufferMap &bufferMap)
    {
      GraphicTask task(GRAPHIC_TASK_UPLOAD);
      task.bufferSlots[0] = &bufferMap;
      addGraphicTask(task);
    }
    
    virtual SDL_Window* getMainSDLWindow() = 0;
    
  public:
    static int GetStereoFlags(const std::string &flags);
    
  private:
    void addTaskToPass(const GraphicTask &task, int pass)
    {
      if (pass >= mTaskPasses.size())
        mTaskPasses.resize(pass+1);
      mTaskPasses.at(pass).push_back(task);
    }
    
  protected:
    void updateTasks();
    
    typedef std::vector<GraphicTask> TaskPass;
    
    TaskPass mPreTasks;
    TaskPass mPostTasks;
    std::vector<TaskPass> mTaskPasses;
    
  private:
    void updateTaskBuffer(void*);
    TaskDelegate mUpdateTaskBufferDelgate;
    
    MultiVector<GraphicTask> mTaskCollection;
    
    TaskPass mTaskBuffer;
    Mutex mTaskBufferMutex;
    
  protected:
    bool addWindows(const XMLTree::Node *node);
    bool addWindow(const XMLTree::Node *node);
  };
}


#endif /* GPU_System_h */
