#include "VulkanIncludes.hpp"


using namespace std;


VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device):
  mDevice(device),
  mVkFramebuffer(VK_NULL_HANDLE),
  mVkRenderPass(VK_NULL_HANDLE) {
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

VkFramebuffer VulkanFrameBuffer::getVkFramebuffer() {
  if (mVkFramebuffer == VK_NULL_HANDLE) {
    vector<VkImageView> attachments;
    for (auto attachment : mColorAttachments)
      attachments.push_back(attachment->getVkImageView());

    VkFramebufferCreateInfo framebufferInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = getVkRenderPass();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = mExtent.width;
    framebufferInfo.height = mExtent.height;
    framebufferInfo.layers = 1;

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mVkFramebuffer) != VK_SUCCESS) {
      cerr << "failed to create framebuffer" << endl;
      mVkFramebuffer = VK_NULL_HANDLE;
    }
  }
  return mVkFramebuffer;
}

VkRenderPass VulkanFrameBuffer::getVkRenderPass() {
  if (mVkRenderPass == VK_NULL_HANDLE) {
    vector<VkAttachmentReference> attachmentRefs;
    vector<VkAttachmentDescription> attachments;

    for (uint32_t index = 0; index < mColorAttachments.size(); ++index) {
      VkAttachmentReference attachmentRef{};
      attachmentRef.attachment = index;
      attachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      attachmentRefs.push_back(attachmentRef);

      VkAttachmentDescription attachment;
      attachment.format         = mColorAttachments[index]->getVkFormat();
      attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
      attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
      attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
      attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
      attachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      attachments.push_back(attachment);
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentRefs.size());
    subpass.pColorAttachments    = attachmentRefs.data();

    VkSubpassDependency dependency;
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &mVkRenderPass) != VK_SUCCESS) {
      cerr << "Error creating Render Pass" << endl;
      mVkRenderPass = VK_NULL_HANDLE;
    }
  }
  return mVkRenderPass;
}

VkViewport VulkanFrameBuffer::getViewport() const {
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)mExtent.width;
  viewport.height = (float)mExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

VkRect2D VulkanFrameBuffer::getScissor() const {
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = mExtent;
  return scissor;
}

void VulkanFrameBuffer::destroy() {
  VkDevice device = mDevice->getVkDevice();
  if (mVkFramebuffer != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(device, mVkFramebuffer, nullptr);
    mVkFramebuffer = VK_NULL_HANDLE;
  }
  if (mVkRenderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(device, mVkRenderPass, nullptr);
    mVkRenderPass = VK_NULL_HANDLE;
  }
  mColorAttachments.clear();
}
