//
//  Vulkan.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/4/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "Vulkan.h"


using namespace std;
using namespace fx;

VkInstance Vulkan::instance;


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

//vector<VkPhysicalDevice> Vulkan::getPhysicalDevices() const {
//  // Get the device count
//  uint32_t deviceCount = 0;
//  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//
//  // Get the device handles
//  vector<VkPhysicalDevice> devices(deviceCount);
//  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//  return devices;
//}

vector<VkQueueFamilyProperties> Vulkan::getQueueFamilyProperties(VkPhysicalDevice device) {
  // Get the queue family count
  uint32_t familyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
  
  // Get the queue family properties
  std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());
  return queueFamilies;
}
