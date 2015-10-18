//
//  MTLGraphicSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef MTLGraphicSystem_h
#define MTLGraphicSystem_h

#include "GraphicSystem.h"
#include <map>

namespace fx
{
  class MTLWindowInterface;
  class MTLFrameInterface;
  class MTLShaderInterface;
  class MTLMeshInterface;
  class MTLTextureInterface;
  
  /**
   *
   */
  class MTLGraphicSystem: public GraphicSystem
  {
  public:
    MTLGraphicSystem();
    virtual ~MTLGraphicSystem();
    
    virtual void update();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual Window*  getWindow(const std::string &name);
    virtual Frame*   getFrame(const std::string &name);
    virtual Shader*  getShader(const std::string &name);
    virtual Mesh*    getMesh(const std::string &name);
    virtual Texture* getTexture(const std::string &name);
    
    virtual InternalUniformMap* createUniformMap();
    
  private:
    void processTasks();
    void processTaskSlot(const TaskSlot &slot);
    void processTask(const GraphicTask &task);
    
    void updateResources();
    
    std::map<std::string, MTLWindowInterface*>  mWindows;
    std::map<std::string, MTLFrameInterface*>   mFrames;
    std::map<std::string, MTLShaderInterface*>  mShaders;
    std::map<std::string, MTLMeshInterface*>    mMeshes;
    std::map<std::string, MTLTextureInterface*> mTextures;
  };
}

#endif /* MetalGraphicSystem_h */
