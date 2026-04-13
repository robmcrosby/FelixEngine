#include <FelixEngine/VulkanTypes.hpp>

#ifndef VulkanFrameBuffer_hpp
#define VulkanFrameBuffer_hpp


namespace Felix {

class VulkanFrameBuffer {
private:
  VulkanDevice*  mDevice;
  VulkanImages   mColorAttachments;
  VkFramebuffers mVkFramebuffers;
  
public:
  VulkanFrameBuffer(VulkanDevice* device);
  ~VulkanFrameBuffer();
  
  void addColorAttachment(VulkanImagePtr image);
  
  VkFramebuffer getVkFramebuffer(VkRenderPass renderPass, int frame);
  
  VkExtent2D getExtent() const;
  
  uint32_t getColorCount() const {return (uint32_t)mColorAttachments.size();}
  void getVkAttachmentReferences(VkAttachmentReferences& references);
  void getVkAttachmentDescriptions(VkAttachmentDescriptions& descriptions);
  
  void destroy();
  void clearVkFramebuffers();
  
private:
  VkFramebuffer createVkFramebuffer(VkRenderPass renderPass, int frame);
};

} /* Felix */

#endif /* VulkanFrameBuffer_hpp */
