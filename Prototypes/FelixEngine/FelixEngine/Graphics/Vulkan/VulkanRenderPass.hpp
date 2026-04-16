#include <FelixEngine/VulkanTypes.hpp>
#include <glm/glm.hpp>

#ifndef VulkanRenderPass_hpp
#define VulkanRenderPass_hpp


namespace Felix {

class VulkanRenderPass {
private:
  VulkanDevice*        mDevice;
  VkRenderPasses       mVkRenderPasses;
  VulkanFrameBufferPtr mFramebuffer;
  glm::vec4            mClearColor;

public:
  VulkanRenderPass(VulkanDevice* device);
  ~VulkanRenderPass();

  void setFramebuffer(VulkanFrameBufferPtr framebuffer);
  VulkanFrameBufferPtr getFrameBuffer() const { return mFramebuffer; }
  
  void setClearColor(const glm::vec4& color) { mClearColor = color; }
  const glm::vec4& clearColor() const { return mClearColor; }
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
