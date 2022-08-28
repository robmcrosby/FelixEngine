
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

string VulkanDevice::type() const {
  switch (mProperties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
      return "Other";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return "Integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return "Discrete GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      return "Virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      return "CPU";
  }
  return "Unknown Device";
}

ostream& operator<<(ostream& os, const VulkanDevicePtr& device) {
  return device->print(os);
}

ostream& VulkanDevice::print(std::ostream& os) const {
  os << name() << " [" << type() << "]";
  return os;
}
