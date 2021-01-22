//
//  MetalGraphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MetalGraphics_h
#define MetalGraphics_h

#include <FelixEngine/Graphics.h>
#include <FelixEngine/Macros.h>


OBJC_CLASS(UIView)

namespace fx {
  
  class MTLGraphicsData;
  
  /** iOS File System */
  class MetalGraphics: public Graphics {
  private:
    MTLGraphicsData  *_data;
    CommandBufferPtr _frameCommandBuffer;
    
  public:
    MetalGraphics();
    virtual ~MetalGraphics();
    
    bool initalize(UIView *view);
    
    virtual FrameBufferPtr   createFrameBuffer();
    virtual ShaderProgramPtr createShaderProgram();
    virtual VertexMeshPtr    createVertexMesh();
    virtual UniformBufferPtr createUniformBuffer();
    virtual TextureBufferPtr createTextureBuffer();
    virtual BufferPoolPtr    createBufferPool();
    virtual RenderPassPtr    createRenderPass();
    virtual CommandBufferPtr createCommandBuffer();
    
    virtual void nextFrame();
    virtual void presentFrame();
    virtual CommandBufferPtr getFrameCommandBuffer();
    
    virtual void setRenderPassToWindow(RenderPassPtr pass, int window);
  };
  
}

#endif /* MetalGraphics_h */
