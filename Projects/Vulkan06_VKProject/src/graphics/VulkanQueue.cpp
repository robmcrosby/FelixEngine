#include <VulkanQueue.hpp>
#include <VulkanDevice.hpp>
#include <VulkanCommand.hpp>
#include <VulkanFrameSync.hpp>

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

  VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, 0};
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
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
  if (auto command = beginSingleCommand()) {
    command->transition(image, newImageLayout, dstAccessMask, dstStageMask);
    command->endSingle();
    waitIdle();
  }
}

void VulkanQueue::copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image) {
  if (auto command = beginSingleCommand()) {
    command->copyBufferToImage(buffer, image);
    command->endSingle();
    waitIdle();
  }
}

void VulkanQueue::copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer) {
  if (auto command = beginSingleCommand()) {
    command->copyImageToBuffer(image, buffer);
    command->endSingle();
    waitIdle();
  }
}

void VulkanQueue::copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst) {
  if (auto command = beginSingleCommand()) {
    command->copyBufferToBuffer(src, dst);
    command->endSingle();
    waitIdle();
  }
}

VulkanCommandPtr VulkanQueue::createCommand(int frames) {
  if (mVkQueue != VK_NULL_HANDLE) {
    VulkanCommandPtr command = make_shared<VulkanCommand>(this, frames);
    return command;
  }
  cerr << "Error: Commands can not be created before Queue initalization" <<  endl;
  return nullptr;
}

VulkanCommandPtr VulkanQueue::beginSingleCommand() {
  if (auto command = createCommand()) {
    command->begin(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    return command;
  }
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
      throw runtime_error("failed to submit command buffer!");
  }
}

void VulkanQueue::submitCommand(VulkanCommandPtr command, VulkanFrameSyncPtr frameSync) {
  if (mVkQueue != VK_NULL_HANDLE) {
    VkSemaphore waitSemaphores[] = {frameSync->availableSemaphore()};
    VkSemaphore signalSemaphores[] = {frameSync->finishedSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkCommandBuffer commandBuffer = command->getVkCommandBuffer(frameSync->currentFrame());
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO, 0};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer,
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    frameSync->resetInFlight();
    if (vkQueueSubmit(mVkQueue, 1, &submitInfo, frameSync->inFlightFence()) != VK_SUCCESS)
      throw runtime_error("failed to submit draw command buffer!");
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
