#include "VulkanIncludes.hpp"


using namespace std;

VulkanInstance::VulkanInstance():
  mVkInstance(VK_NULL_HANDLE),
  mValidationEnabled(false) {
  mApplicationVersion = VK_MAKE_VERSION(1, 0, 0);
  mEngineVersion = VK_MAKE_VERSION(1, 0, 0);
  mApiVersion = VK_API_VERSION_1_0;
}

VulkanInstance::~VulkanInstance() {
  destroy();
}

VulkanInstance& VulkanInstance::Get() {
  static VulkanInstance instance;
  return instance;
}

bool VulkanInstance::init() {
  VkApplicationInfo appInfo{};
  appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName    = mApplicationName.c_str();
  appInfo.applicationVersion  = mApplicationVersion;
  appInfo.pEngineName         = mEngineName.c_str();
  appInfo.engineVersion       = mEngineVersion;
  appInfo.apiVersion          = mApiVersion;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount = (uint32_t)mExtensions.size();
  createInfo.ppEnabledExtensionNames = mExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  createInfo.enabledLayerCount = (uint32_t)mValidationLayers.size();
  createInfo.ppEnabledLayerNames = mValidationLayers.data();
  createInfo.pNext = nullptr;
  // if (mValidationEnabled) {
  //   populateDebugMessengerCreateInfo(debugCreateInfo);
  //   createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  // }

  return vkCreateInstance(&createInfo, nullptr, &mVkInstance) == VK_SUCCESS;
}

void VulkanInstance::destroy() {
  if (mVkInstance != VK_NULL_HANDLE) {
    vkDestroyInstance(mVkInstance, nullptr);
    mVkInstance = VK_NULL_HANDLE;
    cout << "Destroy VulkanInstance" << endl;
  }
}

void VulkanInstance::setApplicationName(StringRef name) {
  if (mVkInstance == VK_NULL_HANDLE)
    mApplicationName = name;
}

void VulkanInstance::setEngineName(StringRef name) {
  if (mVkInstance == VK_NULL_HANDLE)
    mEngineName = name;
}

void VulkanInstance::setApplicationVersion(int major, int minor, int patch) {
  if (mVkInstance == VK_NULL_HANDLE)
    mApplicationVersion = VK_MAKE_VERSION(major, minor, patch);
}

void VulkanInstance::setEngineVersion(int major, int minor, int patch) {
  if (mVkInstance == VK_NULL_HANDLE)
    mEngineVersion = VK_MAKE_VERSION(major, minor, patch);
}

void VulkanInstance::enableValidation() {
  if (mVkInstance == VK_NULL_HANDLE && !mValidationEnabled) {
    #ifndef __APPLE__
      mValidationEnabled = true;
      mValidationLayers.push_back("VK_EXT_debug_utils");
    #endif
  }
}

void VulkanInstance::addExtension(CString extension) {
  if (mVkInstance == VK_NULL_HANDLE)
    mExtensions.push_back(extension);
}
