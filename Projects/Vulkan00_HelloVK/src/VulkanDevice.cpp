
#include "VulkanIncludes.hpp"


using namespace std;


VulkanDevice::VulkanDevice(VkPhysicalDevice device):
  mVkPhysicalDevice(device),
  mVkDevice(VK_NULL_HANDLE) {
  vkGetPhysicalDeviceProperties(mVkPhysicalDevice, &mProperties);
  vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &mFeatures);
}

VulkanDevice::~VulkanDevice() {
  destroy();
}

void VulkanDevice::destroy() {

}

string VulkanDevice::name() const {
  return mProperties.deviceName;
}

ostream& operator<<(ostream& os, const VulkanDevicePtr& device) {
  return device->print(os);
}

ostream& VulkanDevice::print(std::ostream& os) const {
  os << name() << endl;
  return os;
}
