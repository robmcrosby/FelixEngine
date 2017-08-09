//
//  MetalGraphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Graphics.h"
#include "Macros.h"

#ifndef MetalGraphics_h
#define MetalGraphics_h

OBJC_CLASS(UIView)

namespace fx {
  
  class MTLGraphicsData;
  class MetalFrameBuffer;
  
  /** iOS File System */
  class MetalGraphics: public Graphics {
  private:
    MTLGraphicsData  *_data;
    MetalFrameBuffer *_windowBuffer;
    
  public:
    MetalGraphics();
    virtual ~MetalGraphics();
    
    bool initalize(UIView *view);
    bool setWindowBuffer(MetalFrameBuffer *buffer, int index = 0);
    
    virtual FrameBuffer*   getMainWindowBuffer();
    virtual FrameBuffer*   createFrameBuffer();
    virtual ShaderProgram* createShaderProgram();
    virtual VertexMesh*    createVertexMesh();
    virtual UniformBuffer* createUniformBuffer();
    virtual TextureBuffer* createTextureBuffer();
    
    virtual void nextFrame();
    virtual void addTask(const GraphicTask &task);
    virtual void presentFrame();
  };
  
}

#endif /* MetalGraphics_h */
