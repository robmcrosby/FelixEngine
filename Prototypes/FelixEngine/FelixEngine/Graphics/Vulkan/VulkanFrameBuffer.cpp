#include "VulkanFrameBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include <cassert>


using namespace std;
namespace Felix {

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device): mDevice(device) {
  
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  destroy();
}

void VulkanFrameBuffer::addColorAttachment(VulkanImagePtr image) {
  mColorAttachments.push_back(image);
}

void VulkanFrameBuffer::setDepthStencilBuffer(VkFormat format) {
  auto buffer = mDevice->createImage();
  buffer->setFormat(format); // VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
  buffer->setAspect(VK_IMAGE_TILING_OPTIMAL);
  buffer->setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  setDepthStencil(buffer);
}

void VulkanFrameBuffer::setDepthStencil(VulkanImagePtr buffer) {
  mDepthAttachment = buffer;
}

VkFramebuffer VulkanFrameBuffer::getVkFramebuffer(VkRenderPass renderPass, int frame) {
  while (mVkFramebuffers.size() <= frame)
    mVkFramebuffers.push_back(VK_NULL_HANDLE);
  if (mVkFramebuffers.at(frame) == VK_NULL_HANDLE)
    mVkFramebuffers.at(frame) = createVkFramebuffer(renderPass, frame);
  return mVkFramebuffers.at(frame);
}

void VulkanFrameBuffer::getVkAttachmentReferences(VkAttachmentReferences& references) {
  references.reserve(mColorAttachments.size());
  for (uint32_t index = 0; index < mColorAttachments.size(); ++index) {
    VkAttachmentReference& reference = references.emplace_back();
    reference.attachment = index;
    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
}

bool VulkanFrameBuffer::resize(uint32_t width, uint32_t height) {
  bool success = true;
  for (auto attachment : mColorAttachments) {
    if (attachment->size() > 0)
      attachment->clearImages();
    success = attachment->alloc(width, height) && success;
  }
  if (mDepthAttachment) {
    if (mDepthAttachment->size() > 0)
      mDepthAttachment->clearImages();
    success = mDepthAttachment->alloc(width, height) && success;
  }
  return success;
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
  descriptions.reserve(mColorAttachments.size()+1);
  for (auto attachment : mColorAttachments) {
    VkAttachmentDescription& description = descriptions.emplace_back();
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
  }
  if (mDepthAttachment) {
    VkAttachmentDescription& description = descriptions.emplace_back();
    description.flags          = 0;
    description.format         = mDepthAttachment->getVkFormat();
    description.samples        = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    description.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    description.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }
}

VkAttachmentReference VulkanFrameBuffer::getDepthVkAttachmentReference() {
  VkAttachmentReference reference;
  reference.attachment = 1;
  reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  return reference;
}

void VulkanFrameBuffer::destroy() {
  clearVkFramebuffers();
  mColorAttachments.clear();
  mDepthAttachment = nullptr;
}

void VulkanFrameBuffer::clearVkFramebuffers() {
  VkDevice device = mDevice->getVkDevice();
  for (auto framebuffer : mVkFramebuffers) {
    if (framebuffer != VK_NULL_HANDLE)
      vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
  mVkFramebuffers.clear();
}

void VulkanFrameBuffer::updateBufferToExtent(VulkanImagePtr buffer, VkExtent2D extent) const {
  if (buffer->width() != extent.width || buffer->height() != extent.height) {
    if (buffer->frames() > 0)
      buffer->clearImages();
    assert(buffer->alloc(extent.width, extent.height));
  }
}

VkFramebuffer VulkanFrameBuffer::createVkFramebuffer(VkRenderPass renderPass, int frame) {
  VkExtent2D extent = getExtent();
  vector<VkImageView> attachments;
  attachments.reserve(mColorAttachments.size()+1);
  for (auto attachment : mColorAttachments) {
    int index = frame % attachment->frames();
    attachments.emplace_back(attachment->getVkImageView(index));
  }
  if (mDepthAttachment) {
    updateBufferToExtent(mDepthAttachment, extent);
    attachments.emplace_back(mDepthAttachment->getVkImageView());
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

} /* Felix */
