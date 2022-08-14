
#include <iostream>
#include <vulkan/vulkan.h>

using namespace std;


VkInstance createInstance() {
  VkInstance mVkInstance;

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "HelloVK";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount = 0;//(uint32_t)mExtensions.size();
  createInfo.ppEnabledExtensionNames = nullptr;//mExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  createInfo.enabledLayerCount = 0; //(uint32_t)mValidationLayers.size();
  createInfo.ppEnabledLayerNames = nullptr; //mValidationLayers.data();
  createInfo.pNext = nullptr;
  // if (mValidationEnabled) {
  //   populateDebugMessengerCreateInfo(debugCreateInfo);
  //   createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  // }

  if (vkCreateInstance(&createInfo, nullptr, &mVkInstance) != VK_SUCCESS)
    cerr << "failed to create instance!" << endl;
  //   throw runtime_error("failed to create instance!");

  return mVkInstance;
}

int main() {
  createInstance();
  cout << "It Works!" << endl;
  return 0;
}
