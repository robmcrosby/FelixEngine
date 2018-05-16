//
//  Vulkan.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/4/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef Vulkan_h
#define Vulkan_h

#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>


namespace fx {
  
  struct SwapChainBuffer {
    VkImage image;
    VkImageView view;
  };
  
  struct Vulkan {
    static VkInstance instance;
    static VkPhysicalDevice physicalDevice;
    static uint32_t queueFamilyIndex;
    
    static VkSurfaceKHR surface;
    static VkFormat surfaceFormat;
    
    static VkDevice device;
    
    // attempt to move the following to other classes
    static VkQueue graphicsQueue;
    static VkQueue presentQueue;
    
    static VkSwapchainKHR swapChain;
    static VkExtent2D swapChainExtent;
    static std::vector<SwapChainBuffer> swapChainBuffers;
    static std::vector<VkFramebuffer> swapChainFrameBuffers;
    static uint32_t currentSwapBuffer;
    
    static VkRenderPass renderPass;
    static VkPipelineLayout pipelineLayout;
    static VkPipeline graphicsPipeline;
    
    static VkCommandPool commandPool;
    static std::vector<VkCommandBuffer> commandBuffers;
    
    static VkSemaphore imageAvailableSemaphore;
    static VkSemaphore renderFinishedSemaphore;
    
    
    static bool createInstance(const VkApplicationInfo &appInfo, const std::vector<std::string> &layers, const std::vector<std::string> &extensions);
    static bool setDefaultPhysicalDevice();
    static bool setDefaultQueueFamily();
    static bool createSwapChain();
    static bool createLogicalDevice();
    
    static bool initDeviceQueue(float &width, float &height);
    static bool createSemaphores();
    
    static void waitIdle();
    static void cleaup();
    
    static bool checkLayerNames(const std::vector<const char *> &layerNames);
    static bool checkExtensionNames(const std::vector<const char *> &extensionNames);
    
    static std::vector<VkLayerProperties> getAvalibleLayers();
    static std::vector<VkExtensionProperties> getAvalibleExtensions();
    static std::vector<VkPhysicalDevice> getPhysicalDevices();
    static std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice device);
    
    static std::vector<VkSurfaceFormatKHR> getPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface);
    
    static VkFormat getFloatFormatForSize(size_t size);
  };
}

#endif /* Vulkan_h */
