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

VulkanCommandPtr VulkanQueue::createCommand() {
  if (mVkQueue != VK_NULL_HANDLE) {
    VulkanCommandPtr command = make_shared<VulkanCommand>(this);
    return command;
  }
  cerr << "Error: Commands can not be created before Queue initalization" <<  endl;
  return nullptr;
}

void VulkanQueue::destroyCommandPool() {
  if (mVkCommandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(mDevice->getVkDevice(), mVkCommandPool, 0);
    mVkCommandPool = VK_NULL_HANDLE;
  }
}
