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
    
    virtual void handle(const Event &event);
    virtual void render() = 0;
    
    virtual Window* getWindow(const std::string &name) = 0;
    virtual Frame* getFrame(const std::string &name) = 0;
    virtual Shader* getShader(const std::string &name) = 0;
    virtual Mesh* getMesh(const std::string &name) = 0;
    virtual Texture* getTexture(const std::string &name) = 0;
    
    Resource* getResource(const std::string &type, const std::string &name);
    virtual InternalUniformMap* getInternalUniformMap(UniformMap *map) = 0;
    
    void addGraphicTask(const GraphicTask &task) {mTaskCollection.append(task);}
    
    virtual SDL_Window* getMainSDLWindow() = 0;
    
  private:
    void update();
    void clearTaskSlots();
    void loadTaskSlots();
    
  protected:
    typedef MultiVector<GraphicTask> TaskCollection;
    
    typedef std::vector<GraphicTask> TaskBuffer;
    typedef std::vector<GraphicTask> TaskSlot;
    typedef std::vector<TaskSlot> TaskSlots;
    
    TaskCollection mTaskCollection;
    TaskBuffer mTaskBuffer;
    
    TaskSlots mTaskSlots;
    Mutex mTaskSlotsMutex;
    Mutex mUpdateMutex;
    
  protected:
    bool addWindows(const XMLTree::Node *node);
    bool addWindow(const XMLTree::Node *node);
  };
}


#endif /* GPU_System_h */
