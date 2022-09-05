#include "VulkanIncludes.hpp"


using namespace std;


VulkanMemory::VulkanMemory(VulkanDevice* device):
  mDevice(device),
  mMemory(VK_NULL_HANDLE),
  mSize(0) {

}

VulkanMemory::~VulkanMemory() {
  deallocate();
}

bool VulkanMemory::allocate(VkDeviceSize size, VkMemoryPropertyFlags properties) {
  return false;
}

void VulkanMemory::deallocate() {

}
