#include "VulkanIncludes.hpp"


using namespace std;


VulkanQueue::VulkanQueue(VulkanDevice* device, VkQueueFlags flags, uint32_t familyIndex, uint32_t queueIndex):
  mDevice(device),
  mVkQueueFlags(flags),
  mFamilyIndex(familyIndex),
  mQueueIndex(queueIndex),
  mVkQueue(VK_NULL_HANDLE) {

}

VulkanQueue::~VulkanQueue() {
  destroy();
}

void VulkanQueue::init() {
  vkGetDeviceQueue(mDevice->getVkDevice(), mFamilyIndex, mQueueIndex, &mVkQueue);
}

void VulkanQueue::destroy() {
  mVkQueue = VK_NULL_HANDLE;
}
