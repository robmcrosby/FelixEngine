//
//  VulkanGraphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "Graphics.h"
#include "Macros.h"

#ifndef VulkanGraphics_h
#define VulkanGraphics_h

OBJC_CLASS(UIView)

namespace fx {
  
  /**
   *
   */
  class VulkanGraphics: public Graphics {
  private:
    
  public:
    VulkanGraphics();
    virtual ~VulkanGraphics();
    
    bool initalize(UIView *view);
    
    virtual FrameBufferPtr   createFrameBuffer();
    virtual ShaderProgramPtr createShaderProgram();
    virtual VertexMeshPtr    createVertexMesh();
    virtual UniformBufferPtr createUniformBuffer();
    virtual TextureBufferPtr createTextureBuffer();
    
    virtual void nextFrame();
    virtual void addTask(const GraphicTask &task);
    virtual void presentFrame();
  };
}

#endif /* VulkanGraphics_h */
