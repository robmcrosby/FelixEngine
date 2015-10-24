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
  class MTLContextInfo;
  class MTLWindowInterface;
  class MTLFrameInterface;
  class MTLShaderInterface;
  class MTLMeshInterface;
  class MTLTextureInterface;
  class MTLSamplerInterface;
  class MTLUniformMapInterface;
  
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
    
    virtual InternalUniformMap* getInternalUniformMap(UniformMap *map);
    
    MTLContextInfo* getContextInfo() const {return mContextInfo;}
    
  private:
    void setNextWindowDrawables() const;
    void presentWindowDrawables() const;
    
    void processTasks();
    void processTaskSlot(const TaskSlot &slot);
    void processTask(const GraphicTask &task);
    
    void updateResources();
    void updateUniforms();
    
    MTLContextInfo *mContextInfo;
    MTLWindowInterface *mMainWindow;
    
    std::map<std::string, MTLWindowInterface*>  mWindows;
    std::map<std::string, MTLFrameInterface*>   mFrames;
    std::map<std::string, MTLShaderInterface*>  mShaders;
    std::map<std::string, MTLMeshInterface*>    mMeshes;
    std::map<std::string, MTLTextureInterface*> mTextures;
    
    mutable std::map<Sampler, MTLSamplerInterface*> mSamplers;
    std::list<MTLUniformMapInterface*> mMTLUniforms;
  };
}

#endif /* MetalGraphicSystem_h */
