#include "VulkanIncludes.hpp"


using namespace std;


VulkanBuffer::VulkanBuffer(VulkanDevice* device):
  mDevice(device) {

}

VulkanBuffer::~VulkanBuffer() {
  destroy();
}

void VulkanBuffer::destroy() {

}
