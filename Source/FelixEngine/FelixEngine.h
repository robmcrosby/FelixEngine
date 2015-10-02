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

class SDL_Window;

namespace fx
{
  /**
   * Felix Engine Class
   */
  class FelixEngine
  {
  public:
    FelixEngine();
    ~FelixEngine();
    
    bool init(const std::string &settingsFile = "");
    bool loadScene(const std::string &sceneFile);
    
    int runLoop();
    
  private:
    std::map<SYSTEM_TYPE, System*> mSystems;
    std::map<std::string, Scene*>  mScenes;
    
    SDL_Window* mWindow;
  };
}
