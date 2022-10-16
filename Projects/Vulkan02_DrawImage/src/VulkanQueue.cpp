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

void VulkanQueue::transition(VulkanImagePtr image,
                             VkImageLayout  newImageLayout,
                             VkAccessFlags  dstAccessMask,
                             VkPipelineStageFlags dstStageMask) {
  auto command = createCommand();
  command->begin();
  command->transition(image, newImageLayout, dstAccessMask, dstStageMask);
  command->end();
  submitCommand(command);
  waitIdle();
  // VkCommandBuffer commandBuffer = beginSingleTimeCommands();
  // image->transition(commandBuffer, newImageLayout, dstAccessMask, dstStageMask);
  // endSingleTimeCommands(commandBuffer);
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

// VkCommandBuffer VulkanQueue::beginSingleTimeCommands() {
//   VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
//   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//   allocInfo.commandPool = mVkCommandPool;
//   allocInfo.commandBufferCount = 1;
//
//   VkCommandBuffer commandBuffer;
//   vkAllocateCommandBuffers(mDevice->getVkDevice(), &allocInfo, &commandBuffer);
//
//   VkCommandBufferBeginInfo beginInfo{};
//   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//   beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//   vkBeginCommandBuffer(commandBuffer, &beginInfo);
//   return commandBuffer;
// }
//
// void VulkanQueue::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
//   vkEndCommandBuffer(commandBuffer);
//
//   VkSubmitInfo submitInfo{};
//   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//   submitInfo.commandBufferCount = 1;
//   submitInfo.pCommandBuffers = &commandBuffer;
//
//   vkQueueSubmit(mVkQueue, 1, &submitInfo, VK_NULL_HANDLE);
//   vkQueueWaitIdle(mVkQueue);
//
//   vkFreeCommandBuffers(mDevice->getVkDevice(), mVkCommandPool, 1, &commandBuffer);
// }

void VulkanQueue::destroyCommandPool() {
  if (mVkCommandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(mDevice->getVkDevice(), mVkCommandPool, 0);
    mVkCommandPool = VK_NULL_HANDLE;
  }
}
