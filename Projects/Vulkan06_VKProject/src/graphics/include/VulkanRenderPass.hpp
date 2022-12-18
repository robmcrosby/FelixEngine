#include <VulkanTypes.hpp>

#ifndef VulkanRenderPass_hpp
#define VulkanRenderPass_hpp


class VulkanRenderPass {
private:
  VulkanDevice*        mDevice;
  VkRenderPasses       mVkRenderPasses;
  VulkanFrameBufferPtr mFramebuffer;

public:
  VulkanRenderPass(VulkanDevice* device);
  ~VulkanRenderPass();

  void setFramebuffer(VulkanFrameBufferPtr framebuffer);

  VkRenderPass  getVkRenderPass(int frame = 0);
  VkFramebuffer getVkFramebuffer(int frame = 0);

  VkExtent2D getExtent()     const;
  VkViewport getViewport()   const;
  VkRect2D   getScissor()    const;
  uint32_t   getColorCount() const;

  void rebuild();

  void destroy();
  void clearVkRenderPasses();

private:
  VkRenderPass createVkRenderPass();
};


#endif /* VulkanRenderPass_hpp */
