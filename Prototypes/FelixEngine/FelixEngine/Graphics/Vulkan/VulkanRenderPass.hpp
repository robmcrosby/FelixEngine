#include <FelixEngine/VulkanTypes.hpp>

#ifndef VulkanRenderPass_hpp
#define VulkanRenderPass_hpp


namespace Felix {

class VulkanRenderPass {
private:
  VulkanDevice*        mDevice;
  VkRenderPasses       mVkRenderPasses;
  VulkanFrameBufferPtr mFramebuffer;

public:
  VulkanRenderPass(VulkanDevice* device);
  ~VulkanRenderPass();

  void setFramebuffer(VulkanFrameBufferPtr framebuffer);
  VulkanFrameBufferPtr getFrameBuffer() const { return mFramebuffer; }
  
  void getVkClearValues(VkClearValues& clearValues);
  
  VkRenderPass  getVkRenderPass(int frame = 0);
  VkFramebuffer getVkFramebuffer(int frame = 0);

  VkExtent2D getExtent()     const;
  VkViewport getViewport()   const;
  VkRect2D   getScissor()    const;
  uint32_t   getColorCount() const;
  bool       hasDepth()      const;

  void rebuild();

  void destroy();
  void clearVkRenderPasses();

private:
  VkRenderPass createVkRenderPass();
};

} /* Felix */

#endif /* VulkanRenderPass_hpp */
