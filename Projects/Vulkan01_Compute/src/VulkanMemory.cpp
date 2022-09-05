#include "VulkanIncludes.hpp"


using namespace std;


VulkanMemory::VulkanMemory(VulkanDevice* device):
  mDevice(device),
  mDeviceMemory(VK_NULL_HANDLE),
  mMemorySize(0),
  mTypeIndex(0) {

}

VulkanMemory::~VulkanMemory() {
  deallocate();
}

bool VulkanMemory::allocate(VkDeviceSize size, VkMemoryPropertyFlags properties) {
  deallocate();
  if (mDevice->getVkMemoryType(mTypeIndex, mMemoryType, properties)) {
    mMemorySize = size;

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = mMemorySize;
    allocInfo.memoryTypeIndex = mTypeIndex;

    if (vkAllocateMemory(mDevice->getVkDevice(), &allocInfo, nullptr, &mDeviceMemory) == VK_SUCCESS)
      return true;
    cerr << "Error: Unable to Allocate Vulkan Memory" << endl;
  }
  return false;
}

void VulkanMemory::deallocate() {
  if (mDeviceMemory != VK_NULL_HANDLE) {
    vkFreeMemory(mDevice->getVkDevice(), mDeviceMemory, nullptr);
    mDeviceMemory = VK_NULL_HANDLE;
  }
}
