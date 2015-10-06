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
    
    bool init(const std::string &settingsFile = "");
    bool loadScene(const std::string &sceneFile);
    
    int runLoop();
    
  private:
    FelixEngine();
    ~FelixEngine();
    
    bool loadSystems(const XMLTree::Node &node);
    bool loasScene(const XMLTree::Node &node);
    
    void addSystem(System *system);
    
    void updateFrame();
    
    std::map<SYSTEM_TYPE, System*> mSystems;
    std::map<std::string, Scene*>  mScenes;
  };
}
