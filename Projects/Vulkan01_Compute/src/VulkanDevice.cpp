#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1002000
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "VulkanIncludes.hpp"



using namespace std;


VulkanDevice::VulkanDevice(VkPhysicalDevice device):
  mVkPhysicalDevice(device),
  mVkDevice(VK_NULL_HANDLE),
  mVmaAllocator(VK_NULL_HANDLE) {
  // Query Physical Device Properties and Features
  vkGetPhysicalDeviceProperties(mVkPhysicalDevice, &mProperties);
  vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &mFeatures);
  vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &mMemoryProperties);

  // Query the avalible Queue Families for the Device
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &count, nullptr);
  mQueueFamilies.resize(count);
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &count, mQueueFamilies.data());

  // Setup Queue Counts
  for (auto family : mQueueFamilies)
    mQueueFamilyCounts.push_back(family.queueCount);
}

VulkanDevice::~VulkanDevice() {
  destroy();
}

bool VulkanDevice::init() {
  auto& instance = VulkanInstance::Get();

  float queuePriority = 1.0f;
  vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  for (uint32_t family = 0; family < mQueueFamilies.size(); ++family) {
    uint32_t count = mQueueFamilies[family].queueCount - mQueueFamilyCounts[family];
    if (count > 0) {
      VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
      queueCreateInfo.queueFamilyIndex = family;
      queueCreateInfo.queueCount = count;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }
  }

  const CStrings& layers = instance.getEnabledLayers();
  const CStrings& extensions = enabledExtensions();

  VkDeviceCreateInfo createInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &mFeatures;
  createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
  createInfo.ppEnabledLayerNames = layers.data();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (vkCreateDevice(mVkPhysicalDevice, &createInfo, nullptr, &mVkDevice) == VK_SUCCESS) {
    for (auto queue : mQueues) {
      if (!queue->init())
        return false;
    }
    return initVmaAllocator();
  }

  cerr << "Error: Unable to create logical device with queues" << endl;
  return false;
}

void VulkanDevice::destroy() {
  destoryVmaAllocator();
  clearQueues();
  if (mVkDevice != VK_NULL_HANDLE) {
    vkDestroyDevice(mVkDevice, nullptr);
    mVkDevice = VK_NULL_HANDLE;
  }
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

bool VulkanDevice::addExtension(StringRef extension) {
  if (mVkDevice == VK_NULL_HANDLE) {
    const CStrings& extensions = VulkanInstance::Get().getEnabledExtensions();
    for (const auto& avalible : extensions) {
      if (extension == avalible) {
        mEnabledExtensions.push_back(avalible);
        return true;
      }
    }
    cerr << "Warning: Extension " << extension;
    cerr << " not enabled for VulkanDevice" << endl;
  }
  else {
    cerr << "Warning: Extension " << extension;
    cerr << " must be added before VulkanDevice initalization" << endl;
  }
  return false;
}

VulkanQueuePtr VulkanDevice::createQueue(VkQueueFlags flags) {
  if (mVkDevice == VK_NULL_HANDLE) {
    uint32_t familyIndex, queueIndex;
    if (pickQueueFamily(flags, familyIndex, queueIndex)) {
      VulkanQueuePtr queue = make_shared<VulkanQueue>(this, flags, familyIndex, queueIndex);
      mQueues.push_back(queue);
      return queue;
    }
  }
  else {
    cerr << "Error: Queue can not be created after device initalization" << endl;
  }
  return nullptr;
}

VulkanBufferPtr VulkanDevice::createBuffer() {
  VulkanBufferPtr buffer = make_shared<VulkanBuffer>(this);
  return buffer;
}

VulkanLayoutPtr VulkanDevice::createLayout() {
  VulkanLayoutPtr layout = make_shared<VulkanLayout>(this);
  return layout;
}

VulkanPipelinePtr VulkanDevice::createPipeline() {
  VulkanPipelinePtr pipeline = make_shared<VulkanPipeline>(this);
  return pipeline;
}

bool VulkanDevice::getVkMemoryType(uint32_t& index, VkMemoryType& type, VkMemoryPropertyFlags properties) {
  for (int i = 0; i < mMemoryProperties.memoryTypeCount; ++i) {
    if ((mMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      index = i;
      type = mMemoryProperties.memoryTypes[i];
      return true;
    }
  }
  return false;
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

bool VulkanDevice::initVmaAllocator() {
  VmaVulkanFunctions vulkanFunctions = {};
  vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
  vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo allocatorCreateInfo = {};
  allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
  allocatorCreateInfo.physicalDevice = mVkPhysicalDevice;
  allocatorCreateInfo.device = mVkDevice;
  allocatorCreateInfo.instance = VulkanInstance::Get().getVkInstance();
  allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

  return vmaCreateAllocator(&allocatorCreateInfo, &mVmaAllocator) == VK_SUCCESS;
}

void VulkanDevice::destoryVmaAllocator() {
  if (mVmaAllocator != VK_NULL_HANDLE) {
    vmaDestroyAllocator(mVmaAllocator);
    mVmaAllocator = VK_NULL_HANDLE;
  }
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
