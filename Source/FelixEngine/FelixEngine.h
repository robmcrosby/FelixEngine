//
//  FelixEngine.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/30/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include <iostream>
#include <map>

#include "System.h"
#include "Scene.h"
#include "XMLTree.h"


namespace fx
{
  class GraphicSystem;
  
  /**
   * Felix Engine Class
   */
  class FelixEngine
  {
  public:
    static FelixEngine* Instance();
    static System* GetSystem(SYSTEM_TYPE type);
    static GraphicSystem* GetGraphicSystem();
    static Scene* GetScene(const std::string &name);
    
    bool init(const std::string &settingsFile = "");
    
    void addSystem(System *system);
    void clearSystems();
    System* getSystem(SYSTEM_TYPE type);
    
    bool loadScene(const std::string &sceneFile);
    void addScene(Scene *scene);
    void clearScenes();
    void deleteScene(const std::string &name);
    Scene* getScene(const std::string &name);
    
    int runLoop();
    
  private:
    FelixEngine();
    ~FelixEngine();
    
    bool loadSystems(const XMLTree::Node &node);
    
    void updateFrame();
    
    std::map<SYSTEM_TYPE, System*> mSystems;
    std::map<std::string, Scene*>  mScenes;
  };
}
