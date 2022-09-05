#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

#ifndef VulkanIncludes_hpp
#define VulkanIncludes_hpp


typedef const char* CString;
typedef std::vector<CString> CStrings;
typedef std::vector<std::string> Strings;
typedef const std::string& StringRef;

typedef std::vector<VkLayerProperties>       VulkanLayerProperties;
typedef std::vector<VkExtensionProperties>   VulkanExtensionProperties;
typedef std::vector<VkQueueFamilyProperties> VulkanQueueFamilies;

class VulkanDevice;
typedef std::shared_ptr<VulkanDevice> VulkanDevicePtr;
typedef std::vector<VulkanDevicePtr> VulkanDevices;


class VulkanInstance {
private:
  VkInstance mVkInstance;

  std::string mApplicationName;
  std::string mEngineName;

  uint32_t mApplicationVersion;
  uint32_t mEngineVersion;
  uint32_t mApiVersion;

  VulkanLayerProperties     mAvailableLayers;
  VulkanExtensionProperties mAvailableExentions;

  bool mValidationEnabled;
  CStrings mEnabledLayers;
  CStrings mEnabledExtensions;

  bool mDebugMessengerEnabled;
  VkDebugUtilsMessengerEXT mDebugMessenger;

  VulkanDevices mDevices;

private:
  VulkanInstance();
  ~VulkanInstance();

public:
  static VulkanInstance& Get();

  bool init();
  void destroy();

  void setApplicationName(StringRef name);
  void setEngineName(StringRef name);

  void setApplicationVersion(int major, int minor, int patch);
  void setEngineVersion(int major, int minor, int patch);

  void enableValidation();
  bool addValidationLayer(StringRef layer);
  bool addExtension(StringRef extension);

  VulkanDevicePtr pickDevice() const;

  friend std::ostream& operator<<(std::ostream& os, const VulkanInstance& instance);
  std::ostream& print(std::ostream& os) const;

private:
  bool createVkInstance();

  bool setupDebugMessenger();
  void destroyDebugMessenger();
  void initDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  bool loadDevices();
  void clearDevices();
};


class VulkanDevice {
private:
  VkPhysicalDevice mVkPhysicalDevice;

  VkPhysicalDeviceProperties mProperties;
  VkPhysicalDeviceFeatures   mFeatures;
  VulkanQueueFamilies        mQueueFamilies;

  VkPhysicalDeviceMemoryProperties mMemoryProperties;

public:
  VulkanDevice(VkPhysicalDevice device);
  ~VulkanDevice();

  void destroy();

  std::string name() const;
  std::string type() const;

  VkPhysicalDeviceType deviceType() const {return mProperties.deviceType;}

  friend std::ostream& operator<<(std::ostream& os, const VulkanDevicePtr& device);
  std::ostream& print(std::ostream& os) const;
  std::ostream& printQueueFlags(std::ostream& os, VkQueueFlags flags) const;
  std::ostream& printHeapFlags(std::ostream& os, VkMemoryHeapFlags flags) const;
  std::ostream& printMemoryFlags(std::ostream& os, VkMemoryPropertyFlags flags) const;
};


#endif /* VulkanIncludes_hpp */