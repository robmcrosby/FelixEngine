#include "VulkanIncludes.hpp"


using namespace std;


VulkanImage::VulkanImage(VulkanDevice* device):
  mDevice(device) {

}

VulkanImage::~VulkanImage() {
  destroy();
}

void VulkanImage::destroy() {

}
