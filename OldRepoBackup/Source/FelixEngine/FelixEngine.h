//
//  FelixEngine.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/30/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//


#include "EventHandler.h"
#include "System.h"
#include "XMLTree.h"

#include "FileSystem.h"
#include "TaskingSystem.h"
#include "GraphicSystem.h"
#include "EventSystem.h"
#include "MotionSystem.h"

#ifndef FelixEngine_h
#define FelixEngine_h

namespace fx
{
  class System;
  class Scene;
  
  /**
   * Felix Engine Class
   */
  class FelixEngine: public EventHandler
  {
  private:
    FelixEngine();
    ~FelixEngine();
    
  public:
    virtual void handle(const Event &event);
    
  public:
    static FelixEngine* Instance();
    
    static System* GetSystem(SYSTEM_TYPE type) {return Instance()->getSystem(type);}
    static TaskingSystem* GetTaskingSystem() {return Instance()->getTaskingSystem();}
    static GraphicSystem* GetGraphicSystem() {return Instance()->getGraphicSystem();}
    static EventSystem* GetEventSystem() {return Instance()->getEventSystem();}
    static MotionSystem* GetMotionSystem() {return Instance()->getMotionSystem();}
    
    static Scene* GetScene(const std::string &name) {return Instance()->getScene(name);}
    
    bool init(const std::string &settingsFile = "");
    
    void addSystem(System *system);
    void clearSystems();
    System* getSystem(SYSTEM_TYPE type) {return mSystems.count(type) ? mSystems.at(type) : nullptr;}
    TaskingSystem* getTaskingSystem() {return static_cast<TaskingSystem*>(getSystem(SYSTEM_TASKING));}
    GraphicSystem* getGraphicSystem() {return static_cast<GraphicSystem*>(getSystem(SYSTEM_GRAPHICS));}
    EventSystem* getEventSystem() {return static_cast<EventSystem*>(getSystem(SYSTEM_EVENTS));}
    MotionSystem* getMotionSystem() {return static_cast<MotionSystem*>(getSystem(SYSTEM_MOTION));}
    
    Scene* loadScene(const File &file);
    Scene* loadScene(const std::string &filePath) {return loadScene(FileSystem::GetResources()+filePath);}
    
    void addScene(Scene *scene);
    void clearScenes();
    void deleteScene(const std::string &name);
    
    Scene* getScene(const std::string &name) {return mScenes.count(name) ? mScenes.at(name) : nullptr;}
    
    int runLoop();
    
    void exit() {mShutdown = true;}
    
  private:
    bool loadSystems(const XMLTree::Node &node);
    void updateFrame();
    static void UpdateFrame(void *ptr);
    
    bool mShutdown;
    std::map<SYSTEM_TYPE, System*> mSystems;
    std::map<std::string, Scene*>  mScenes;
  };
}

#endif /* FelixEngine_h */
