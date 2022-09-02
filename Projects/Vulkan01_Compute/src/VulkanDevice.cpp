
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
  for (auto family : mQueueFamilies)
    mQueueFamilyCounts.push_back(family.queueCount);
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
  uint32_t familyIndex, queueIndex;
  if (pickQueueFamily(flags, familyIndex, queueIndex)) {
    VulkanQueuePtr queue = make_shared<VulkanQueue>(this, flags, familyIndex, queueIndex);
    mQueues.push_back(queue);
    return queue;
  }
  return nullptr;
}

ostream& operator<<(ostream& os, const VulkanDevicePtr& device) {
  return device->print(os);
}

ostream& VulkanDevice::print(ostream& os) const {
  os << name() << endl;
  os << type() << endl;
  os << "Queue Families:" << endl;

  int queueIndex = 0;
  for (auto family : mQueueFamilies) {
    os << "  Queue[" << queueIndex++ << "] count: " << family.queueCount << " ";
    printQueueFlags(os, family.queueFlags);
    os << endl;
    // os << "  Queue[" << queueIndex++ << "] count: " << family.queueCount << " [";
    // if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    //   os << "Graphics ";
    // if (family.queueFlags & VK_QUEUE_COMPUTE_BIT)
    //   os << "Compute ";
    // if (family.queueFlags & VK_QUEUE_TRANSFER_BIT)
    //   os << "Transfer ";
    // if (family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
    //   os << "Sparse ";
    // os << "]" << endl;
  }
  return os;
}

ostream& VulkanDevice::printQueueFlags(ostream& os, VkQueueFlags flags) const {
  os << "[";
  if (flags & VK_QUEUE_GRAPHICS_BIT)
    os << "Graphics ";
  if (flags & VK_QUEUE_COMPUTE_BIT)
    os << "Compute ";
  if (flags & VK_QUEUE_TRANSFER_BIT)
    os << "Transfer ";
  if (flags & VK_QUEUE_SPARSE_BINDING_BIT)
    os << "Sparse ";
  os << "]";
  return os;
}

bool VulkanDevice::pickQueueFamily(VkQueueFlags flags, uint32_t& familyIndex, uint32_t& queueIndex) {
  bool found = false;
  for (uint32_t i = 0; i < mQueueFamilies.size(); ++i) {
    if ((mQueueFamilies[i].queueFlags & flags) == flags && mQueueFamilyCounts[i] > 0) {
      familyIndex = i;
      found = true;
    }
  }

  if (found) {
    queueIndex = mQueueFamilies[familyIndex].queueCount - mQueueFamilyCounts[familyIndex];
    mQueueFamilyCounts[familyIndex] -= 1;
    cout << "Found family: " << familyIndex << " at queue index: " << queueIndex << endl;
  }
  else {
    cerr << "Error: No queue family avalible with flags: ";
    printQueueFlags(cerr, flags);
  }
  return found;
}

void VulkanDevice::clearQueues() {
  for (auto queue : mQueues)
    queue->destroy();
  mQueues.clear();
}
