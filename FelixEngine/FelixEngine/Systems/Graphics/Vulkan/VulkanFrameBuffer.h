//
//  VulkanFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanFrameBuffer_h
#define VulkanFrameBuffer_h

#include "VulkanFrameAttachment.h"
#include <map>


namespace fx {
  typedef std::shared_ptr<VulkanFrameAttachment> AttachmentPtr;
  typedef std::vector<AttachmentPtr> Attachments;
  
  /**
   *
   */
  class VulkanFrameBuffer: public FrameBuffer {
  private:
    ivec2 _frameSize;
    std::map<int, VkPipeline> _pipelines;
    
    std::map<uint32_t, VkRenderPass> _renderPasses;
    std::vector<VkFramebuffer> _swapChainFramebuffers;
    VkPipelineLayout _pipelineLayout;
    
    Attachments _colorAttachments;
    AttachmentPtr _depthAttachment;
    
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
    
    VkRenderPass getRenderPass(const GraphicTask &task);
    VkFramebuffer getFrameBuffer(const GraphicTask &task);
    VkPipelineLayout getPipelineLayout();
    
    bool hasDepthBuffer() const {return _depthAttachment.get() != nullptr;}
    int totalColorAttachements() const {return (int)_colorAttachments.size();}
    
  private:
    void clearBuffers();
    VkFormat findDepthFormat();
    bool createRenderPass(const GraphicTask &task);
    bool createFrameBuffers(const GraphicTask &task);
    
    static VkAttachmentLoadOp getAttachmentLoadOp(LOAD_ACTION action);
    static VkAttachmentStoreOp getAttachmentStoreOp(STORE_ACTION action);
  };
}

#endif /* VulkanFrameBuffer_h */
