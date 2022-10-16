#include "VulkanIncludes.hpp"


using namespace std;


VulkanQueue::VulkanQueue(VulkanDevice* device, VkQueueFlags flags, uint32_t familyIndex, uint32_t queueIndex):
  mDevice(device),
  mVkQueueFlags(flags),
  mFamilyIndex(familyIndex),
  mQueueIndex(queueIndex),
  mVkQueue(VK_NULL_HANDLE),
  mVkCommandPool(VK_NULL_HANDLE) {

}

VulkanQueue::~VulkanQueue() {
  destroy();
}

bool VulkanQueue::init() {
  VkDevice device = mDevice->getVkDevice();
  vkGetDeviceQueue(device, mFamilyIndex, mQueueIndex, &mVkQueue);

  VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, 0, 0};
  commandPoolCreateInfo.queueFamilyIndex = mFamilyIndex;
  if (vkCreateCommandPool(device, &commandPoolCreateInfo, 0, &mVkCommandPool) != VK_SUCCESS) {
    cerr << "Error creating Command Pool" << endl;
    return false;
  }
  return true;
}

void VulkanQueue::destroy() {
  destroyCommandPool();
  mVkQueue = VK_NULL_HANDLE;
}

void VulkanQueue::transition(
  VulkanImagePtr image,
  VkImageLayout  newImageLayout,
  VkAccessFlags  dstAccessMask,
  VkPipelineStageFlags dstStageMask
) {
  auto command = createCommand();
  command->begin();
  command->transition(image, newImageLayout, dstAccessMask, dstStageMask);
  command->end();
  submitCommand(command);
  waitIdle();
}

void VulkanQueue::copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer) {
  auto command = createCommand();
  command->begin();
  command->copyImageToBuffer(image, buffer);
  command->end();
  submitCommand(command);
  waitIdle();
}

VulkanCommandPtr VulkanQueue::createCommand() {
  if (mVkQueue != VK_NULL_HANDLE) {
    VulkanCommandPtr command = make_shared<VulkanCommand>(this);
    return command;
  }
  cerr << "Error: Commands can not be created before Queue initalization" <<  endl;
  return nullptr;
}

void VulkanQueue::submitCommand(VulkanCommandPtr command) {
  if (mVkQueue != VK_NULL_HANDLE) {
    VkCommandBuffer commandBuffer = command->getVkCommandBuffer();
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO, 0};
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = 0;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer,
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = 0;

    if (vkQueueSubmit(mVkQueue, 1, &submitInfo, 0) != VK_SUCCESS)
      cerr << "Error Submiting Command to Queue" << endl;
  }
}

void VulkanQueue::waitIdle() {
  if (mVkQueue != VK_NULL_HANDLE)
    vkQueueWaitIdle(mVkQueue);
}

void VulkanQueue::destroyCommandPool() {
  if (mVkCommandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(mDevice->getVkDevice(), mVkCommandPool, 0);
    mVkCommandPool = VK_NULL_HANDLE;
  }
}
