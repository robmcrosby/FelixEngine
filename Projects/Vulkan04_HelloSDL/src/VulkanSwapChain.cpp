#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "VulkanIncludes.hpp"


using namespace std;


VulkanSwapChain::VulkanSwapChain(VulkanDevice* device):
  mDevice(device),
  mSdlWindow(nullptr),
  mSurface(VK_NULL_HANDLE) {

}

VulkanSwapChain::~VulkanSwapChain() {
  destroy();
}

void VulkanSwapChain::setToWindow(SDL_Window *window) {
  mSdlWindow = window;

  VkInstance instance = mDevice->getVkInstance();
  if (!SDL_Vulkan_CreateSurface(mSdlWindow, instance, &mSurface))
    throw runtime_error("failed to create window surface!");
}

void VulkanSwapChain::destroy() {
  destroySurface();
}

void VulkanSwapChain::destroySurface() {
  if (mSurface != VK_NULL_HANDLE) {
    VkInstance instance = mDevice->getVkInstance();
    vkDestroySurfaceKHR(instance, mSurface, nullptr);
    mSurface = VK_NULL_HANDLE;
  }
}
