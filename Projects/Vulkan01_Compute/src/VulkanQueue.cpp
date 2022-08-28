#include "VulkanIncludes.hpp"


using namespace std;


VulkanQueue::VulkanQueue(VulkanDevice* device, VkQueueFlags flags):
  mDevice(device),
  mVkQueueFlags(flags) {
  cout << "Created Queue" << endl;
}

VulkanQueue::~VulkanQueue() {
  destroy();
}

void VulkanQueue::destroy() {
  cout << "Queue Destroyed" << endl;
}
