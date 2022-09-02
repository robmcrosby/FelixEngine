#include "VulkanIncludes.hpp"


using namespace std;


VulkanQueue::VulkanQueue(VulkanDevice* device, VkQueueFlags flags, uint32_t familyIndex, uint32_t queueIndex):
  mDevice(device),
  mVkQueueFlags(flags),
  mFamilyIndex(familyIndex),
  mQueueIndex(queueIndex) {
  cout << "Created Queue" << endl;
}

VulkanQueue::~VulkanQueue() {
  destroy();
}

void VulkanQueue::destroy() {
  cout << "Queue Destroyed" << endl;
}
