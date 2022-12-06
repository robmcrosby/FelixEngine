#include "VulkanIncludes.hpp"


using namespace std;


VulkanRenderPass::VulkanRenderPass(VulkanDevice* device):
  mDevice(device) {

}

VulkanRenderPass::~VulkanRenderPass() {
  destroy();
}

void VulkanRenderPass::setFramebuffer(VulkanFrameBufferPtr framebuffer) {
  mFramebuffer = framebuffer;
}

VkRenderPass VulkanRenderPass::getVkRenderPass(int frame) {
  while (mVkRenderPasses.size() <= frame)
    mVkRenderPasses.push_back(VK_NULL_HANDLE);
  if (mVkRenderPasses.at(frame) == VK_NULL_HANDLE)
    mVkRenderPasses.at(frame) = createVkRenderPass();
  return mVkRenderPasses.at(frame);
}

VkFramebuffer VulkanRenderPass::getVkFramebuffer(int frame) {
  return mFramebuffer->getVkFramebuffer(getVkRenderPass(frame), frame);
}

VkExtent2D VulkanRenderPass::getExtent() const {
  return mFramebuffer ? mFramebuffer->getExtent() : VkExtent2D{0, 0};
}

VkViewport VulkanRenderPass::getViewport() const {
  VkExtent2D extent = getExtent();
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

VkRect2D VulkanRenderPass::getScissor()  const {
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = getExtent();
  return scissor;
}

uint32_t VulkanRenderPass::getColorCount() const {
  return mFramebuffer ? mFramebuffer->getColorCount() : 0;
}

void VulkanRenderPass::rebuild() {
  clearVkRenderPasses();
  mFramebuffer->clearVkFramebuffers();
}

void VulkanRenderPass::destroy() {
  clearVkRenderPasses();
  mFramebuffer = nullptr;
}

void VulkanRenderPass::clearVkRenderPasses() {
  VkDevice device = mDevice->getVkDevice();
  for (auto renderPass : mVkRenderPasses) {
    if (renderPass != VK_NULL_HANDLE)
      vkDestroyRenderPass(device, renderPass, nullptr);
  }
  mVkRenderPasses.clear();
}

VkRenderPass VulkanRenderPass::createVkRenderPass() {
  VkAttachmentReferences attachmentRefs;
  VkAttachmentDescriptions attachments;

  mFramebuffer->getVkAttachmentReferences(attachmentRefs);
  mFramebuffer->getVkAttachmentDescriptions(attachments);

  VkSubpassDescription subpass{};
  subpass.flags                = 0;
  subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentRefs.size());
  subpass.pColorAttachments    = attachmentRefs.data();

  VkSubpassDependency dependency;
  dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass      = 0;
  dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask   = 0;
  dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = 0;

  VkRenderPassCreateInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderPassInfo.flags           = 0;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments    = attachments.data();
  renderPassInfo.subpassCount    = 1;
  renderPassInfo.pSubpasses      = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies   = &dependency;

  VkDevice device = mDevice->getVkDevice();
  VkRenderPass renderPass = VK_NULL_HANDLE;
  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    cerr << "Error creating Render Pass" << endl;
    renderPass = VK_NULL_HANDLE;
  }
  return renderPass;
}
