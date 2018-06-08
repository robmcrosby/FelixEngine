//
//  VulkanFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanFrameBuffer_h
#define VulkanFrameBuffer_h

#include "VulkanTextureBuffer.h"
#include <map>


namespace fx {
  
  
  class VulkanFrameBuffer: public FrameBuffer {
  private:
    ivec2 _frameSize;
    std::map<int, VkPipeline> _pipelines;
    
    VkRenderPass _renderPass;
    std::vector<VkFramebuffer> _swapChainFramebuffers;
    VkPipelineLayout _pipelineLayout;
    
    std::shared_ptr<VulkanTextureBuffer> _depthBuffer;
    
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
    
    VkPipeline getVkPipelineForTask(const GraphicTask &task);
    
    VkRenderPass getRenderPass() {return _renderPass;}
    VkFramebuffer getFrameBuffer(int index) {return _swapChainFramebuffers[index];}
    VkPipelineLayout getPipelineLayout() {return _pipelineLayout;}
    
  private:
    void clearBuffers();
    VkFormat findDepthFormat();
    bool createRenderPass();
    bool createFrameBuffers();
  };
}

#endif /* VulkanFrameBuffer_h */
