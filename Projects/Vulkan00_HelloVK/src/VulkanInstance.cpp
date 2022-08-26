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
  mDebugMessenger(VK_NULL_HANDLE),
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
  bool initalized = createVkInstance();
  if (initalized) {
    initalized = !mValidationEnabled || setupDebugMessenger();
    initalized = initalized && loadDevices();

    if (!initalized)
      destroy();
  }
  return initalized;
}

void VulkanInstance::destroy() {
  if (mVkInstance != VK_NULL_HANDLE) {
    clearDevices();
    destroyDebugMessenger();
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
      //mValidationLayers.push_back("VK_EXT_debug_utils");
      mValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
      mExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

      cout << "Avalible Layers:" << endl;
      vector<VkLayerProperties> layers;
      getAvailableLayers(layers);
      for (const auto& properties : layers) {
        cout << "  " << properties.layerName << endl;
      }

      cout << "Avalible Extensions:" << endl;
      vector<VkExtensionProperties> extensions;
      getAvailableExtensions(extensions);
      for (const auto& extension : extensions) {
        cout << "  " << extension.extensionName << endl;
      }
    #endif
  }
}

void VulkanInstance::addExtension(CString extension) {
  if (mVkInstance == VK_NULL_HANDLE)
    mExtensions.push_back(extension);
}

void VulkanInstance::getAvailableLayers(vector<VkLayerProperties> &layers) {
  uint32_t count;
  vkEnumerateInstanceLayerProperties(&count, nullptr);

  layers.resize(count);
  vkEnumerateInstanceLayerProperties(&count, layers.data());
}

void VulkanInstance::getAvailableExtensions(vector<VkExtensionProperties> &extensions) {
  uint32_t count;
  vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

  extensions.resize(count);
  vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
}

bool VulkanInstance::createVkInstance() {
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
    initDebugMessengerInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  }

  bool err = vkCreateInstance(&createInfo, nullptr, &mVkInstance) != VK_SUCCESS;

  if (err)
    cerr << "Failed to create Vulkan Instance" << endl;
  return !err;
}

bool VulkanInstance::setupDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  initDebugMessengerInfo(createInfo);

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugUtilsMessengerEXT");
  bool err = func == nullptr || func(mVkInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS;

  if (err)
    cerr << "Failed to set up Debug Messenger Callback" << endl;
  return !err;
}

void VulkanInstance::destroyDebugMessenger() {
  if (mVkInstance != VK_NULL_HANDLE && mDebugMessenger != VK_NULL_HANDLE) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(mVkInstance, "vkDestroyDebugUtilsMessengerEXT");
    func(mVkInstance, mDebugMessenger, nullptr);
    mDebugMessenger = VK_NULL_HANDLE;
  }
}

void VulkanInstance::initDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
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

bool VulkanInstance::loadDevices() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, nullptr);

  bool err = deviceCount == 0;
  if (!err) {
    vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, devices.data());
    for (auto physicalDevice : devices) {
      cout << "Device: " << physicalDevice << endl;
      // auto device = make_shared<VulkanDevice>(this, physicalDevice);
      // mDevices.push_back(device);
    }
  }

  if (err)
    cerr << "Failed to find Devices with Vulkan support" << endl;
  return !err;
}

void VulkanInstance::clearDevices() {
  // for (auto device : mDevices)
  //   device->cleanup();
  // mDevices.clear();
}
