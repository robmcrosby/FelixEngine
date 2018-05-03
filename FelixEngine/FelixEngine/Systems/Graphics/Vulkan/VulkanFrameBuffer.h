//
//  VulkanFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanFrameBuffer_h
#define VulkanFrameBuffer_h

#include "GraphicResources.h"


namespace fx {
  class VulkanFrameBuffer: public FrameBuffer {
  private:
    
  public:
    VulkanFrameBuffer();
    virtual ~VulkanFrameBuffer();
    
    virtual bool loadXML(const XMLTree::Node &node);
    virtual bool setToWindow(int index);
    
    virtual bool resize(int width, int height);
    virtual ivec2 size() const;
    
    virtual bool addDepthBuffer();
    virtual bool addColorTexture();
    
    virtual TextureBufferPtr getColorTexture(int index);
  };
}

#endif /* VulkanFrameBuffer_h */
