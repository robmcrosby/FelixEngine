#include "VulkanIncludes.hpp"


using namespace std;


VulkanSwapChain::VulkanSwapChain(VulkanDevice* device): mDevice(device) {

}

VulkanSwapChain::~VulkanSwapChain() {
  destroy();
}

void VulkanSwapChain::destroy() {

}
