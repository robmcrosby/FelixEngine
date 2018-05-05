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
  struct Vulkan {
    static VkInstance instance;
    
    static bool createInstance(const VkApplicationInfo &appInfo, const std::vector<std::string> &layers, const std::vector<std::string> &extensions);
    
    static bool checkLayerNames(const std::vector<const char *> &layerNames);
    static bool checkExtensionNames(const std::vector<const char *> &extensionNames);
    
    static std::vector<VkLayerProperties> getAvalibleLayers();
    static std::vector<VkExtensionProperties> getAvalibleExtensions();
    static std::vector<VkPhysicalDevice> getPhysicalDevices();
    static std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice device);
  };
}

#endif /* Vulkan_h */
