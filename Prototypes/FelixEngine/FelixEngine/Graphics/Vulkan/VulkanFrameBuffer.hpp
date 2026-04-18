#include <FelixEngine/VulkanTypes.hpp>

#ifndef VulkanFrameBuffer_hpp
#define VulkanFrameBuffer_hpp


namespace Felix {

class VulkanFrameBuffer {
private:
  VulkanDevice*  mDevice;
  VulkanImages   mColorAttachments;
  VulkanImagePtr mDepthAttachment;
  VkFramebuffers mVkFramebuffers;
  
public:
  VulkanFrameBuffer(VulkanDevice* device);
  ~VulkanFrameBuffer();
  
  void addColorAttachment(VulkanImagePtr image);
  VulkanImagePtr getColorAttachment(int index = 0) const { return mColorAttachments.at(index); }
  
  void setDepthStencilBuffer(VkFormat format);
  void setDepthStencil(VulkanImagePtr buffer);
  VulkanImagePtr getDepthAttachment() const { return mDepthAttachment; }
  
  VkFramebuffer getVkFramebuffer(VkRenderPass renderPass, int frame);
  
  bool resize(uint32_t width, uint32_t height);
  bool resize(VkExtent2D extent) { return resize(extent.width, extent.height); }
  VkExtent2D getExtent() const;
  
  uint32_t getColorCount() const {return (uint32_t)mColorAttachments.size();}
  void getVkAttachmentReferences(VkAttachmentReferences& references);
  void getVkAttachmentDescriptions(VkAttachmentDescriptions& descriptions);
  
  bool hasDepthStencil() const { return mDepthAttachment != nullptr; }
  VkAttachmentReference getDepthVkAttachmentReference();
  
  void destroy();
  void clearVkFramebuffers();
  
private:
  void updateBufferToExtent(VulkanImagePtr buffer, VkExtent2D extent) const;
  VkFramebuffer createVkFramebuffer(VkRenderPass renderPass, int frame);
};

} /* Felix */

#endif /* VulkanFrameBuffer_hpp */
