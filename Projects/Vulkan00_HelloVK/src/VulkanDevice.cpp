
#include "VulkanIncludes.hpp"


using namespace std;


VulkanDevice::VulkanDevice(VkPhysicalDevice device):
  mVkPhysicalDevice(device) {
  vkGetPhysicalDeviceProperties(mVkPhysicalDevice, &mProperties);
  vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &mFeatures);
  vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &mMemoryProperties);

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

ostream& operator<<(ostream& os, const VulkanDevicePtr& device) {
  return device->print(os);
}

ostream& VulkanDevice::print(std::ostream& os) const {
  os << name() << endl;
  os << type() << endl;

  os << "Queue Families:" << endl;
  int queueIndex = 0;
  for (auto family : mQueueFamilies) {
    os << "  Queue[" << queueIndex++ << "] count:" << family.queueCount << " ";
    printQueueFlags(os, family.queueFlags);
    os << endl;
  }

  os << "Memory Heaps:" << endl;
  for (int i = 0; i < mMemoryProperties.memoryHeapCount; ++i) {
    os << "  Heap[" << i << "] ";
    os << "Size:" << mMemoryProperties.memoryHeaps[i].size << " bytes ";
    printHeapFlags(os, mMemoryProperties.memoryHeaps[i].flags);
    os << endl;
  }

  os << "Memory Types:" << endl;
  for (int i = 0; i < mMemoryProperties.memoryTypeCount; ++i) {
    os << "  Type Index:" << i;
    os << " Heap Index:" << mMemoryProperties.memoryTypes[i].heapIndex << " ";
    printMemoryFlags(os, mMemoryProperties.memoryTypes[i].propertyFlags);
    os << endl;
  }

  return os;
}

ostream& VulkanDevice::printQueueFlags(ostream& os, VkQueueFlags flags) const {
  os << "[ ";
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

ostream& VulkanDevice::printHeapFlags(ostream& os, VkMemoryHeapFlags flags) const {
  os << "[ ";
  if (flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
    os << "Device ";
  if (flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT)
    os << "Multi ";
  os << "]";
  return os;
}

ostream& VulkanDevice::printMemoryFlags(ostream& os, VkMemoryPropertyFlags flags) const {
  os << "[ ";
  if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    os << "Device ";
  if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    os << "Visible ";
  if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    os << "Coherent ";
  if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
    os << "Cached ";
  if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
    os << "Lazy ";
  if (flags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
    os << "!Protected ";
  if (flags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD)
    os << "!AMD_Coherent ";
  if (flags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD)
    os << "!AMD_Uncached ";
  if (flags & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV)
    os << "!RDMA ";
  os << "]";
  return os;
}
