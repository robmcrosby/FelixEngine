//
//  VulkanGraphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanGraphics_h
#define VulkanGraphics_h

#include "Graphics.h"
#include "Macros.h"
#include <vector>


OBJC_CLASS(UIView)

namespace fx {
  class VulkanCommandPool;
  
  typedef std::vector<GraphicTask> RenderPass;
  typedef std::vector<VulkanCommandPool*> CommandPools;
  
  /**
   *
   */
  class VulkanGraphics: public Graphics {
  private:
    std::vector<std::string> _layers;
    std::vector<std::string> _extensions;
    
    std::vector<RenderPass> _renderPasses;
    CommandPools _commandPools;
    
  public:
    VulkanGraphics();
    virtual ~VulkanGraphics();
    
    bool initalize(UIView *view);
    
    virtual FrameBufferPtr   createFrameBuffer();
    virtual ShaderProgramPtr createShaderProgram();
    virtual VertexMeshPtr    createVertexMesh();
    virtual UniformBufferPtr createUniformBuffer();
    virtual TextureBufferPtr createTextureBuffer();
    virtual BufferPoolPtr    createBufferPool();
    
    virtual void nextFrame();
    virtual void addTask(const GraphicTask &task);
    virtual void presentFrame();
    
  private:
    bool createInstance();
    bool createSwapChain(UIView *view);
    bool createCommandPool();
    
    void createCommandPools();
    void clearCommandPools();
  };
}

#endif /* VulkanGraphics_h */
