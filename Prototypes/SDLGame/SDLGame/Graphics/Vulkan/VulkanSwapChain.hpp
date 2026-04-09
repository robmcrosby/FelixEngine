#include "VulkanTypes.hpp"
#include "VulkanImage.hpp"

#ifndef VulkanSwapChain_hpp
#define VulkanSwapChain_hpp

#define MAX_FRAMES_IN_FLIGHT 2


class SDL_Window;

class VulkanSwapChain {
private:
  VulkanDevice*  mDevice;
  SDL_Window*    mSdlWindow;
  VkSurfaceKHR   mVkSurface;
  VkSwapchainKHR mVkSwapChain;
  uint32_t       mImageCount;
  uint32_t       mCurrentFrame;
  VulkanImagePtr mPresentImage;

  VkSurfaceFormatKHR mVkSurfaceFormat;
  VkPresentModeKHR   mVkPrsentMode;

public:
  VulkanSwapChain(VulkanDevice* device);
  ~VulkanSwapChain();

  void setToWindow(SDL_Window *window);
  SDL_Window* getWindow() const {return mSdlWindow;}

  VkSurfaceCapabilitiesKHR getVkSurfaceCapabilities() const;
  VkSurfaceFormats         getVkSurfaceFormats() const;
  VkPresentModes           getVkPresentModes() const;

  VkSurfaceFormatKHR pickSurfaceFormat() const;
  VkPresentModeKHR   pickPresentMode() const;
  uint32_t           pickImageCount() const;
  
  uint32_t minImageCount() const;
  uint32_t imageCount() const {return mImageCount;}

  VkExtent2D getExtent() const;
  uint32_t frames() const {return !mPresentImage ? 0 : mPresentImage->frames();}

  VulkanImagePtr getPresentImage();

  int getNextFrame(VkSemaphore semaphore);
  int getNextFrame(VulkanFrameSyncPtr frameSync);
  void presentFrame(uint32_t frame, VkSemaphore semaphore, VkQueue queue);
  void presentFrame(VulkanFrameSyncPtr frameSync, VulkanQueuePtr queue);

  void rebuild();
  void destroy();

private:
  void createSwapChain();
  VkSwapchainKHR createVkSwapChain(
    VkSurfaceFormatKHR surfaceFormat,
    VkPresentModeKHR   presentMode,
    uint32_t           imageCount,
    VkExtent2D         extent,
    VkSwapchainKHR     oldSwapchain
  ) const;

  void destroySwapChain();
  void destroySurface();
};


#endif /* VulkanSwapChain_hpp */
