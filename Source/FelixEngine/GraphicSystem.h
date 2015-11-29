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
    virtual InternalUniformMap* getInternalUniformMap(const UniformMap *map) = 0;
    
    void addGraphicTask(const GraphicTask &task) {mTaskCollection.append(task);}
    virtual SDL_Window* getMainSDLWindow() = 0;
    
  public:
    static int GetStereoFlags(const std::string &flags);
    
  private:
    void update();
    void clearPasses();
    void loadPasses();
    
  protected:
    typedef MultiVector<GraphicTask> TaskCollection;
    
    typedef std::vector<GraphicTask> Pass;
    typedef std::vector<Pass> Passes;
    
    TaskCollection mTaskCollection;
    Pass mTaskBuffer;
    
    Passes mPasses;
    Mutex mPassesMutex;
    Mutex mUpdateMutex;
    
  protected:
    bool addWindows(const XMLTree::Node *node);
    bool addWindow(const XMLTree::Node *node);
  };
}


#endif /* GPU_System_h */
