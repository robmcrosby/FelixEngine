#include <VulkanInstance.hpp>
#include <VulkanDevice.hpp>

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
  mValidationEnabled(false),
  mDebugMessengerEnabled(false),
  mApiVersion(VK_API_VERSION_1_2) {
  mApplicationVersion = VK_MAKE_VERSION(1, 0, 0);
  mEngineVersion = VK_MAKE_VERSION(1, 0, 0);

  uint32_t count;

  vkEnumerateInstanceLayerProperties(&count, nullptr);
  mAvailableLayers.resize(count);
  vkEnumerateInstanceLayerProperties(&count, mAvailableLayers.data());

  vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
  mAvailableExentions.resize(count);
  vkEnumerateInstanceExtensionProperties(nullptr, &count, mAvailableExentions.data());
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
    initalized = !mDebugMessengerEnabled || setupDebugMessenger();
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
    mValidationEnabled = true;
    addValidationLayer("VK_LAYER_KHRONOS_validation");
    mDebugMessengerEnabled = addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
}

bool VulkanInstance::addValidationLayer(StringRef layer) {
  if (mVkInstance == VK_NULL_HANDLE) {
    for (const auto& layerProperties : mAvailableLayers) {
      if (layer == layerProperties.layerName) {
        mEnabledLayers.push_back(layerProperties.layerName);
        return true;
      }
    }
    //cerr << "Warning: Validation Layer " << layer;
    //cerr << " not avalible for this version of Vulkan" << endl;
  }
  else {
    cerr << "Warning: Validation Layer " << layer;
    cerr << " must be added before VulkanInstance initalization" << endl;
  }
  return false;
}

bool VulkanInstance::addExtension(StringRef extension) {
  if (mVkInstance == VK_NULL_HANDLE) {
    for (const auto& extensionProperties : mAvailableExentions) {
      if (extension == extensionProperties.extensionName) {
        mEnabledExtensions.push_back(extensionProperties.extensionName);
        return true;
      }
    }
    cerr << "Warning: Extension " << extension;
    cerr << " not avalible for this version of Vulkan" << endl;
  }
  else {
    cerr << "Warning: Extension " << extension;
    cerr << " must be added before VulkanInstance initalization" << endl;
  }
  return false;
}

VulkanDevicePtr VulkanInstance::pickDevice() const {
  for (auto device : mDevices) {
    if (device->deviceType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      return device;
  }
  for (auto device : mDevices) {
    if (device->deviceType() == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
      return device;
  }
  return nullptr;
}

ostream& operator<<(ostream& os, const VulkanInstance& instance) {
  return instance.print(os);
}

ostream& VulkanInstance::print(ostream& os) const {
  os << "Avalible Layers:" << endl;
  for (const auto& layer : mAvailableLayers)
    os << "  " << layer.layerName << endl;

  os << "Avalible Extensions:" << endl;
  for (const auto& extension : mAvailableExentions)
    os << "  " << extension.extensionName << endl;

  os << "Devices:" << endl;
  for (auto device : mDevices)
    os << "  " << device->name() << " [" << device->type() << "]" << endl;

  return os;
}

bool VulkanInstance::createVkInstance() {
  VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.pApplicationName    = mApplicationName.c_str();
  appInfo.applicationVersion  = mApplicationVersion;
  appInfo.pEngineName         = mEngineName.c_str();
  appInfo.engineVersion       = mEngineVersion;
  appInfo.apiVersion          = mApiVersion;

  VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount = (uint32_t)mEnabledExtensions.size();
  createInfo.ppEnabledExtensionNames = mEnabledExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  createInfo.enabledLayerCount = (uint32_t)mEnabledLayers.size();
  createInfo.ppEnabledLayerNames = mEnabledLayers.data();
  createInfo.pNext = nullptr;
  if (mDebugMessengerEnabled) {
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
  createInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
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
      auto device = make_shared<VulkanDevice>(physicalDevice);
      mDevices.push_back(device);
    }
  }

  if (err)
    cerr << "Failed to find Devices with Vulkan support" << endl;
  return !err;
}

void VulkanInstance::clearDevices() {
  for (auto device : mDevices)
    device->destroy();
  mDevices.clear();
}
