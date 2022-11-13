#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <algorithm>
#include "VulkanIncludes.hpp"


using namespace std;


VulkanSwapChain::VulkanSwapChain(VulkanDevice* device):
  mDevice(device),
  mSdlWindow(nullptr),
  mVkSurface(VK_NULL_HANDLE),
  mVkSwapChain(VK_NULL_HANDLE),
  mImageCount(0) {

}

VulkanSwapChain::~VulkanSwapChain() {
  destroy();
}

void VulkanSwapChain::setToWindow(SDL_Window *window) {
  mSdlWindow = window;

  auto instance = mDevice->getVkInstance();
  if (!SDL_Vulkan_CreateSurface(mSdlWindow, instance, &mVkSurface))
    throw runtime_error("failed to create window surface!");
}

VkSurfaceCapabilitiesKHR VulkanSwapChain::getVkSurfaceCapabilities() const {
  VkSurfaceCapabilitiesKHR capabilities;
  auto physicalDevice = mDevice->getVkPhysicalDevice();
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, mVkSurface, &capabilities);
  return capabilities;
}

VkSurfaceFormats VulkanSwapChain::getVkSurfaceFormats() const {
  uint32_t count;
  auto device = mDevice->getVkPhysicalDevice();
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, mVkSurface, &count, nullptr);

  VkSurfaceFormats formats(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, mVkSurface, &count, formats.data());
  return formats;
}

VkPresentModes VulkanSwapChain::getVkPresentModes() const {
  uint32_t count;
  auto device = mDevice->getVkPhysicalDevice();
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, mVkSurface, &count, nullptr);

  VkPresentModes modes(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, mVkSurface, &count, modes.data());
  return modes;
}

VkSurfaceFormatKHR VulkanSwapChain::pickSurfaceFormat() const {
  auto surfaceFormats = getVkSurfaceFormats();
  for (const auto& format : surfaceFormats) {
      if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        return format;
  }
  return surfaceFormats.front();
}

VkPresentModeKHR VulkanSwapChain::pickPresentMode() const {
  auto presentModes = getVkPresentModes();
  for (const auto& presentMode : presentModes) {
      if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        return presentMode;
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t VulkanSwapChain::pickImageCount() const {
  auto capabilities = getVkSurfaceCapabilities();
  uint32_t imageCount = capabilities.minImageCount + 1;

  if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    return capabilities.maxImageCount;
  return imageCount;
}

VkExtent2D VulkanSwapChain::getExtent() const {
  auto capabilities = getVkSurfaceCapabilities();
  if (capabilities.currentExtent.width != UINT32_MAX)
      return capabilities.currentExtent;

  int width, height;
  SDL_GetWindowSize(mSdlWindow, &width, &height);

  VkExtent2D extent = {
    static_cast<uint32_t>(width),
    static_cast<uint32_t>(height)
  };
  extent.width = clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
  extent.height = clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
  return extent;
}

VulkanImagePtr VulkanSwapChain::getPresentImage() {
  if (mPresentImage == nullptr)
    createSwapChain();
  return mPresentImage;
}

void VulkanSwapChain::destroy() {
  destroySwapChain();
  destroySurface();
}

void VulkanSwapChain::createSwapChain() {
  mVkSurfaceFormat = pickSurfaceFormat();
  mVkPrsentMode = pickPresentMode();
  mImageCount = pickImageCount();
  auto extent = getExtent();

  mVkSwapChain = createVkSwapChain(
    mVkSurfaceFormat,
    mVkPrsentMode,
    mImageCount, extent
  );

  auto device = mDevice->getVkDevice();
  vkGetSwapchainImagesKHR(device, mVkSwapChain, &mImageCount, nullptr);
  cout << mImageCount << " Swap Chain Images" << endl;
  VkImages swapChainImages(mImageCount);
  vkGetSwapchainImagesKHR(device, mVkSwapChain, &mImageCount, swapChainImages.data());

  mPresentImage = mDevice->createImage();
}

VkSwapchainKHR VulkanSwapChain::createVkSwapChain(
  VkSurfaceFormatKHR surfaceFormat,
  VkPresentModeKHR   presentMode,
  uint32_t           imageCount,
  VkExtent2D         extent
) const {
  auto capabilities = getVkSurfaceCapabilities();

  VkSwapchainCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, 0, 0};
  createInfo.surface = mVkSurface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform = capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 0;
  createInfo.pQueueFamilyIndices = nullptr;

  VkDevice device = mDevice->getVkDevice();
  VkSwapchainKHR swapChain = VK_NULL_HANDLE;
  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
    throw runtime_error("failed to create swap chain!");
  return swapChain;
}

void VulkanSwapChain::destroySwapChain() {
  if (mVkSwapChain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(mDevice->getVkDevice(), mVkSwapChain, nullptr);
    mVkSwapChain = VK_NULL_HANDLE;
  }
}

void VulkanSwapChain::destroySurface() {
  if (mVkSurface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(mDevice->getVkInstance(), mVkSurface, nullptr);
    mVkSurface = VK_NULL_HANDLE;
  }
}
