
#include "VulkanIncludes.hpp"


using namespace std;


VulkanDevice::VulkanDevice(VkPhysicalDevice device):
  mVkPhysicalDevice(device) {
  vkGetPhysicalDeviceProperties(mVkPhysicalDevice, &mProperties);
  vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &mFeatures);

  // Query the avalible Queue Families for the Device
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &count, nullptr);
  mQueueFamilies.resize(count);
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &count, mQueueFamilies.data());
}

VulkanDevice::~VulkanDevice() {
  destroy();
}

void VulkanDevice::destroy() {
  clearQueues();
}

string VulkanDevice::name() const {
  return mProperties.deviceName;
}

string VulkanDevice::type() const {
  switch (mProperties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return "Integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return "Discrete GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      return "Virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      return "CPU";
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
      return "Other";
  }
  return "Unknown Device";
}

VulkanQueuePtr VulkanDevice::createQueue(VkQueueFlags flags) {
  VulkanQueuePtr queue = make_shared<VulkanQueue>(this, flags);
  mQueues.push_back(queue);
  return queue;
}

ostream& operator<<(ostream& os, const VulkanDevicePtr& device) {
  return device->print(os);
}

ostream& VulkanDevice::print(std::ostream& os) const {
  os << name() << endl;
  os << type() << endl;
  os << "Queue Families:" << endl;

  int queueIndex = 0;
  for (auto family : mQueueFamilies) {
    os << "  Queue[" << queueIndex++ << "] count: " << family.queueCount << " ["; // << family.queueFlags << endl;
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      os << "Graphics ";
    if (family.queueFlags & VK_QUEUE_COMPUTE_BIT)
      os << "Compute ";
    if (family.queueFlags & VK_QUEUE_TRANSFER_BIT)
      os << "Transfer ";
    if (family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
      os << "Sparse ";
    os << "]" << endl;
  }
  return os;
}

void VulkanDevice::clearQueues() {
  for (auto queue : mQueues)
    queue->destroy();
  mQueues.clear();
}
