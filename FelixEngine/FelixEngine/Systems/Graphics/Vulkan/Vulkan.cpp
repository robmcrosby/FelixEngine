//
//  Vulkan.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/4/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "Vulkan.h"
#include "FileSystem.h"


using namespace std;
using namespace fx;

VkInstance Vulkan::instance;
VkPhysicalDevice Vulkan::physicalDevice;
uint32_t Vulkan::queueFamilyIndex;

VkSurfaceKHR Vulkan::surface;
VkFormat Vulkan::surfaceFormat;

VkDevice Vulkan::device;
VkQueue Vulkan::graphicsQueue;
VkQueue Vulkan::presentQueue;

VkSwapchainKHR Vulkan::swapChain;
VkExtent2D Vulkan::swapChainExtent;
vector<SwapChainBuffer> Vulkan::swapChainBuffers;
vector<VkFramebuffer> Vulkan::swapChainFrameBuffers;
uint32_t Vulkan::currentBuffer;

VkRenderPass Vulkan::renderPass;
VkPipelineLayout Vulkan::pipelineLayout;
VkPipeline Vulkan::graphicsPipeline;

VkCommandPool Vulkan::commandPool;
std::vector<VkCommandBuffer> Vulkan::commandBuffers;

VkSemaphore Vulkan::imageAvailableSemaphore;
VkSemaphore Vulkan::renderFinishedSemaphore;


bool Vulkan::createInstance(const VkApplicationInfo &appInfo, const vector<string> &layers, const vector<string> &extensions) {
  // Define the layer names
  vector<const char *> layerNames;
  for (const string &layer : layers)
    layerNames.push_back(layer.c_str());
  if (!checkLayerNames(layerNames))
    return false;
  
  // Define the extension names
  vector<const char *> extensionNames;// = {"VK_MVK_moltenvk"};
  for (const string &extension : extensions)
    extensionNames.push_back(extension.c_str());
  if (!checkExtensionNames(extensionNames))
    return false;
  
  // Provide the Instance Info for layers and extensions
  VkInstanceCreateInfo createInfo;
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = (uint32_t)layerNames.size();
  createInfo.ppEnabledLayerNames = layerNames.size() > 0 ? layerNames.data() : nullptr;
  createInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
  createInfo.ppEnabledExtensionNames = extensionNames.size() > 0 ? extensionNames.data() : nullptr;
  
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    cerr << "Error creating Vulkan Instance" << endl;
    return false;
  }
  return true;
}

bool Vulkan::setDefaultPhysicalDevice() {
  // Get the Pysical Devices
  vector<VkPhysicalDevice> devices = getPhysicalDevices();
  if (devices.size() == 0) {
    cerr << "No Physical Devices Found" << endl;
    return false;
  }
  
  // Use First Device
  physicalDevice = devices.front();
  return true;
}

bool Vulkan::setDefaultQueueFamily() {
  vector<VkQueueFamilyProperties> queueFamilies = getQueueFamilyProperties(physicalDevice);
  if (queueFamilies.size() == 0) {
    cerr << "No Queue Faimly Properties Found for Physical Device" << endl;
    return true;
  }
  
  // Pick the first family for now
  queueFamilyIndex = 0;
  return true;
}

bool Vulkan::createLogicalDevice() {
  // Definee the Queue information
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;
  
  // Define the Create Logical Device Properties
  VkPhysicalDeviceFeatures deviceFeatures = {};
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;
  createInfo.enabledLayerCount = 0;
  
  // Create the Logical Device
  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
    cerr << "failed to create logical device!" << endl;
    return false;
  }
  
  return true;
}

bool Vulkan::initDeviceQueue(float &width, float &height) {
  // Get the Graphics and Presentation Queue(s)
  vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
  presentQueue = graphicsQueue;
  
  // Get the Surface Capabilites
  VkSurfaceCapabilitiesKHR surfCapabilities;
  assert(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCapabilities) == VK_SUCCESS);
  
  // Get the Present Modes for the Surface
  uint32_t presentModeCount;
  assert(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL) == VK_SUCCESS);
  vector<VkPresentModeKHR> presentModes(presentModeCount);
  assert(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()) == VK_SUCCESS);
  
  // Determine the size extents of the buffers
  // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
  if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
    // If the surface size is undefined, the size is set to
    // the size of the images requested.
    swapChainExtent.width = width;
    swapChainExtent.height = height;
    if (swapChainExtent.width < surfCapabilities.minImageExtent.width)
      swapChainExtent.width = surfCapabilities.minImageExtent.width;
    else if (swapChainExtent.width > surfCapabilities.maxImageExtent.width)
      swapChainExtent.width = surfCapabilities.maxImageExtent.width;
    
    if (swapChainExtent.height < surfCapabilities.minImageExtent.height)
      swapChainExtent.height = surfCapabilities.minImageExtent.height;
    else if (swapChainExtent.height > surfCapabilities.maxImageExtent.height)
      swapChainExtent.height = surfCapabilities.maxImageExtent.height;
  }
  else {
    // If the surface size is defined, the swap chain size must match
    swapChainExtent = surfCapabilities.currentExtent;
    width = swapChainExtent.width;
    height = swapChainExtent.height;
  }
  
  // Set to First In First Out Presentation Mode
  VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  
  // Get the desired number of buffers to get at any one time
  uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;
  
  //
  VkSurfaceTransformFlagBitsKHR preTransform;
  if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  else
    preTransform = surfCapabilities.currentTransform;
  
  // Determine the Alpha Bit
  VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };
  for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
    if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
      compositeAlpha = compositeAlphaFlags[i];
      break;
    }
  }
  
  // Define the Swap Chain Info
  VkSwapchainCreateInfoKHR swapChainInfo = {};
  swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainInfo.pNext = NULL;
  swapChainInfo.surface = surface;
  swapChainInfo.minImageCount = desiredNumberOfSwapChainImages;
  swapChainInfo.imageFormat = surfaceFormat;
  swapChainInfo.imageExtent.width = swapChainExtent.width;
  swapChainInfo.imageExtent.height = swapChainExtent.height;
  swapChainInfo.preTransform = preTransform;
  swapChainInfo.compositeAlpha = compositeAlpha;
  swapChainInfo.imageArrayLayers = 1;
  swapChainInfo.presentMode = swapchainPresentMode;
  swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
#ifndef __ANDROID__
  swapChainInfo.clipped = true;
#else
  swapChainInfo.clipped = false;
#endif
  swapChainInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  swapChainInfo.imageUsage = 0;
  swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapChainInfo.queueFamilyIndexCount = 0;
  swapChainInfo.pQueueFamilyIndices = NULL;
  //uint32_t queueFamilyIndices[2] = {(uint32_t)queueFamilyIndex, (uint32_t)queueFamilyIndex};
  
  // Create the Swap Chain
  if (vkCreateSwapchainKHR(device, &swapChainInfo, NULL, &swapChain) != VK_SUCCESS) {
    cerr << "Error Creating Swap Chain" << endl;
    return false;
  }
  
  // Get the Swap Chain Images
  uint32_t swapChainImageCount;
  assert(vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, NULL) == VK_SUCCESS);
  vector<VkImage> swapChainImages(swapChainImageCount);
  assert(vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages.data()) == VK_SUCCESS);
  
  for (uint32_t i = 0; i < swapChainImageCount; i++) {
    SwapChainBuffer buffer;
    
    VkImageViewCreateInfo colorImageView = {};
    colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorImageView.pNext = NULL;
    colorImageView.format = surfaceFormat;
    colorImageView.components.r = VK_COMPONENT_SWIZZLE_R;
    colorImageView.components.g = VK_COMPONENT_SWIZZLE_G;
    colorImageView.components.b = VK_COMPONENT_SWIZZLE_B;
    colorImageView.components.a = VK_COMPONENT_SWIZZLE_A;
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.flags = 0;
    
    buffer.image = swapChainImages[i];
    colorImageView.image = buffer.image;
    
    assert(vkCreateImageView(device, &colorImageView, NULL, &buffer.view) == VK_SUCCESS);
    swapChainBuffers.push_back(buffer);
  }
  return true;
}

bool Vulkan::createSemaphores() {
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  
  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
    cerr << "Error Creating Vulkan Semaphores" << endl;
    return false;
  }
  return true;
}

bool Vulkan::checkLayerNames(const vector<const char *> &layerNames) {
  bool success = true;
  vector<VkLayerProperties> avalibleLayers = getAvalibleLayers();
  for (const auto &name : layerNames) {
    // Check if the layer names are avalible in the list
    bool layerAvalible = false;
    for (const auto &layer : avalibleLayers) {
      if (strcmp(name, layer.layerName) == 0) {
        layerAvalible = true;
        break;
      }
    }
    
    // Print out any missing layers
    if (!layerAvalible)
      cerr << "Layer " << name << " Not avalible on this device" << endl;
    success &= layerAvalible;
  }
  return success;
}

void Vulkan::cleaup() {
  if (device != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(device);
    
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    
    vkDestroyCommandPool(device, commandPool, nullptr);
    
    for (auto framebuffer : swapChainFrameBuffers)
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    
    vkDestroyDevice(device, nullptr);
    device = VK_NULL_HANDLE;
  }
}

bool Vulkan::checkExtensionNames(const std::vector<const char *> &extensionNames) {
  bool success = true;
  vector<VkExtensionProperties> avalibleExtensions = getAvalibleExtensions();
  for (const auto &name : extensionNames) {
    // Check if the layer names are avalible in the list
    bool extensionAvalible = false;
    for (const auto &extension : avalibleExtensions) {
      if (strcmp(name, extension.extensionName) == 0) {
        extensionAvalible = true;
        break;
      }
    }
    
    // Print out any missing layers
    if (!extensionAvalible)
      cerr << "Extension " << name << " Not avalible on this device" << endl;
    success &= extensionAvalible;
  }
  return success;
}

VkShaderModule Vulkan::createShaderModule(const std::string &fileName) {
  FileData code;
  assert(FileSystem::loadData(code, fileName));
  
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
  
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    cerr << "Error Creating Shader Module" << endl;
    assert(false);
  }
  return shaderModule;
}

vector<VkLayerProperties> Vulkan::getAvalibleLayers() {
  // Get the avalible layer count
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  
  // Retreive the avalible layers
  vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  return availableLayers;
}

vector<VkExtensionProperties> Vulkan::getAvalibleExtensions() {
  // Get the avalible extension count
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  
  // Retreive the avalible extensions
  vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
  return availableExtensions;
}

vector<VkPhysicalDevice> Vulkan::getPhysicalDevices() {
  // Get the device count
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  // Get the device handles
  vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  return devices;
}

vector<VkQueueFamilyProperties> Vulkan::getQueueFamilyProperties(VkPhysicalDevice device) {
  // Get the queue family count
  uint32_t familyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
  
  // Get the queue family properties
  std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());
  return queueFamilies;
}

vector<VkSurfaceFormatKHR> Vulkan::getPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface) {
  uint32_t formatCount;
  assert(vkGetPhysicalDeviceSurfaceFormatsKHR(Vulkan::physicalDevice, Vulkan::surface, &formatCount, NULL) == VK_SUCCESS);
  
  vector<VkSurfaceFormatKHR> formats(formatCount);
  assert(vkGetPhysicalDeviceSurfaceFormatsKHR(Vulkan::physicalDevice, Vulkan::surface, &formatCount, formats.data()) == VK_SUCCESS);
  
  return formats;
}
