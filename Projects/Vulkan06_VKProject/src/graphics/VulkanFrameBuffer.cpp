#include <VulkanIncludes.hpp>


using namespace std;


VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device): mDevice(device) {

}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  destroy();
}

void VulkanFrameBuffer::addColorAttachment(VulkanImagePtr image) {
  mColorAttachments.push_back(image);
}

VkFramebuffer VulkanFrameBuffer::getVkFramebuffer(VkRenderPass renderPass, int frame) {
  while (mVkFramebuffers.size() <= frame)
    mVkFramebuffers.push_back(VK_NULL_HANDLE);
  if (mVkFramebuffers.at(frame) == VK_NULL_HANDLE)
    mVkFramebuffers.at(frame) = createVkFramebuffer(renderPass, frame);
  return mVkFramebuffers.at(frame);
}

void VulkanFrameBuffer::getVkAttachmentReferences(VkAttachmentReferences& references) {
  for (uint32_t index = 0; index < mColorAttachments.size(); ++index) {
    VkAttachmentReference reference;
    reference.attachment = index;
    reference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    references.push_back(reference);
  }
}

VkExtent2D VulkanFrameBuffer::getExtent() const {
  VkExtent2D extent = {0, 0};
  if (!mColorAttachments.empty()) {
    extent.width = mColorAttachments.front()->width();
    extent.height = mColorAttachments.front()->height();
  }
  return extent;
}

void VulkanFrameBuffer::getVkAttachmentDescriptions(VkAttachmentDescriptions& descriptions) {
  for (auto attachment : mColorAttachments) {
    VkAttachmentDescription description;
    description.flags          = 0;
    description.format         = attachment->getVkFormat();
    description.samples        = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    description.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    description.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    description.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (attachment->isSwapImage())
      description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    descriptions.push_back(description);
  }
}

void VulkanFrameBuffer::destroy() {
  clearVkFramebuffers();
  mColorAttachments.clear();
}

void VulkanFrameBuffer::clearVkFramebuffers() {
  VkDevice device = mDevice->getVkDevice();
  for (auto framebuffer : mVkFramebuffers) {
    if (framebuffer != VK_NULL_HANDLE)
      vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
  mVkFramebuffers.clear();
}

VkFramebuffer VulkanFrameBuffer::createVkFramebuffer(VkRenderPass renderPass, int frame) {
  VkExtent2D extent = getExtent();
  vector<VkImageView> attachments;
  for (auto attachment : mColorAttachments) {
    int index = frame % attachment->frames();
    attachments.push_back(attachment->getVkImageView(index));
  }

  VkFramebufferCreateInfo framebufferInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = extent.width;
  framebufferInfo.height = extent.height;
  framebufferInfo.layers = 1;

  VkDevice device = mDevice->getVkDevice();
  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
    cerr << "failed to create framebuffer" << endl;
    framebuffer = VK_NULL_HANDLE;
  }
  return framebuffer;
}
