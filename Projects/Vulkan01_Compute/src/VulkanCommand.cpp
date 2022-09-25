#include "VulkanIncludes.hpp"
#include <fstream>

using namespace std;


VulkanCommand::VulkanCommand(VulkanQueue* queue):
  mQueue(queue),
  mVkCommandPool(VK_NULL_HANDLE),
  mVkCommandBuffer(VK_NULL_HANDLE) {
  alloc();
}

VulkanCommand::~VulkanCommand() {
  destroy();
}

bool VulkanCommand::alloc() {
  if (mVkCommandBuffer == VK_NULL_HANDLE) {
    mVkCommandPool = mQueue->getVkCommandPool();

    VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, 0};
    allocInfo.commandPool = mVkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    allocInfo.commandBufferCount = 1;

    VkDevice device = mQueue->getDevice()->getVkDevice();
    if (vkAllocateCommandBuffers(device, &allocInfo, &mVkCommandBuffer) != VK_SUCCESS) {
      cerr << "Error Allocating Command Buffer" << endl;
      mVkCommandBuffer = VK_NULL_HANDLE;
    }
  }
  return mVkCommandBuffer != VK_NULL_HANDLE;
}

bool VulkanCommand::begin() {
  VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, 0};
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = 0;

  return mVkCommandBuffer != VK_NULL_HANDLE && vkBeginCommandBuffer(mVkCommandBuffer, &beginInfo) == VK_SUCCESS;
}

void VulkanCommand::end() {
  if (mVkCommandBuffer != VK_NULL_HANDLE)
    vkEndCommandBuffer(mVkCommandBuffer);
}

void VulkanCommand::free() {
  if (mVkCommandBuffer != VK_NULL_HANDLE) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkFreeCommandBuffers(device, mVkCommandPool, 1, &mVkCommandBuffer);
    mVkCommandPool = VK_NULL_HANDLE;
    mVkCommandBuffer = VK_NULL_HANDLE;
  }
}

void VulkanCommand::destroy() {
  free();
}
