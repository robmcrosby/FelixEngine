#include "VulkanIncludes.hpp"


using namespace std;


VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device):
  mDevice(device),
  mVkFramebuffer(VK_NULL_HANDLE) {
  mExtent.width = 0;
  mExtent.height = 0;
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  destroy();
}

void VulkanFrameBuffer::addColorAttachment(VulkanImagePtr image) {
  mExtent.width  = image->getWidth();
  mExtent.height = image->getHeight();
  mColorAttachments.push_back(image);
}

VkFramebuffer VulkanFrameBuffer::getVkFramebuffer(VkRenderPass renderPass) {
  if (mVkFramebuffer == VK_NULL_HANDLE)
    mVkFramebuffer = createVkFramebuffer(renderPass);
  return mVkFramebuffer;
}

void VulkanFrameBuffer::getVkAttachmentReferences(VkAttachmentReferences& references) {
  for (uint32_t index = 0; index < mColorAttachments.size(); ++index) {
    VkAttachmentReference reference;
    reference.attachment = index;
    reference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    references.push_back(reference);
  }
}

void VulkanFrameBuffer::getVkAttachmentDescriptions(VkAttachmentDescriptions& descriptions) {
  for (auto attachment : mColorAttachments) {
    VkAttachmentDescription description;
    description.format         = attachment->getVkFormat();
    description.samples        = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    description.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    description.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    description.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    descriptions.push_back(description);
  }
}

void VulkanFrameBuffer::destroy() {
  if (mVkFramebuffer != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyFramebuffer(device, mVkFramebuffer, nullptr);
    mVkFramebuffer = VK_NULL_HANDLE;
  }
  mColorAttachments.clear();
}

VkFramebuffer VulkanFrameBuffer::createVkFramebuffer(VkRenderPass renderPass) {
  vector<VkImageView> attachments;
  for (auto attachment : mColorAttachments)
    attachments.push_back(attachment->getVkImageView());

  VkFramebufferCreateInfo framebufferInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = mExtent.width;
  framebufferInfo.height = mExtent.height;
  framebufferInfo.layers = 1;

  VkDevice device = mDevice->getVkDevice();
  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
    cerr << "failed to create framebuffer" << endl;
    framebuffer = VK_NULL_HANDLE;
  }
  return framebuffer;
}
