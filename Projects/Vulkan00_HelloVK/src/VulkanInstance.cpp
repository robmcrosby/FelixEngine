#include "VulkanIncludes.hpp"


using namespace std;


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData) {
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    cerr << "validation layer: " << pCallbackData->pMessage << endl;
  return VK_FALSE;
}


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
  if (mValidationEnabled) {
    initDebugMessenger(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  }

  return vkCreateInstance(&createInfo, nullptr, &mVkInstance) == VK_SUCCESS;
}

void VulkanInstance::destroy() {
  if (mVkInstance != VK_NULL_HANDLE) {
    vkDestroyInstance(mVkInstance, nullptr);
    mVkInstance = VK_NULL_HANDLE;
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

void VulkanInstance::initDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}
