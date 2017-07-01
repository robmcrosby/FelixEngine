//
//  MetalGraphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Graphics.h"
#include "Macros.h"

OBJC_CLASS(UIView)


namespace fx {
  
  class MTLGraphicsData;
  class MetalFrameBuffer;
  
  /** iOS File System */
  class MetalGraphics: public Graphics {
  private:
    MTLGraphicsData  *_data;
    MetalFrameBuffer *_frame;
    
  public:
    MetalGraphics();
    virtual ~MetalGraphics();
    
    bool initalize(UIView *view);
    
    virtual FrameBuffer*   getMainWindowBuffer();
    virtual ShaderProgram* createShaderProgram();
    virtual VertexMesh*    createVertexMesh();
    virtual UniformBuffer* createUniformBuffer();
    
    virtual void nextFrame();
    virtual void addTask(const GraphicTask &task);
    virtual void presentFrame();
  };
  
}
