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

class VulkanQueue;
typedef std::shared_ptr<VulkanQueue> VulkanQueuePtr;
typedef std::vector<VulkanQueuePtr> VulkanQueues;


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

  const CStrings& getEnabledLayers() const {return mEnabledLayers;}
  const CStrings& getEnabledExtensions() const {return mEnabledExtensions;}

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
  VkDevice         mVkDevice;

  VkPhysicalDeviceProperties mProperties;
  VkPhysicalDeviceFeatures   mFeatures;
  CStrings                   mEnabledExtensions;
  VulkanQueueFamilies        mQueueFamilies;
  std::vector<uint32_t>      mQueueFamilyCounts;

  VulkanQueues mQueues;

public:
  VulkanDevice(VkPhysicalDevice device);
  ~VulkanDevice();

  bool init();
  void destroy();

  std::string name() const;
  std::string type() const;

  VkPhysicalDeviceType deviceType() const {return mProperties.deviceType;}
  VkDevice getVkDevice() const {return mVkDevice;}

  bool addExtension(StringRef extension);
  const CStrings& enabledExtensions() const {return mEnabledExtensions;}

  VulkanQueuePtr createQueue(VkQueueFlags flags);

  friend std::ostream& operator<<(std::ostream& os, const VulkanDevicePtr& device);
  std::ostream& print(std::ostream& os) const;
  std::ostream& printQueueFlags(std::ostream& os, VkQueueFlags flags) const;

private:
  bool pickQueueFamily(VkQueueFlags flags, uint32_t& familyIndex, uint32_t& queueIndex);
  void clearQueues();
};


class VulkanQueue {
private:
  VulkanDevice* mDevice;
  VkQueueFlags  mVkQueueFlags;
  uint32_t      mFamilyIndex;
  uint32_t      mQueueIndex;
  VkQueue       mVkQueue;

public:
  VulkanQueue(VulkanDevice* device, VkQueueFlags flags, uint32_t familyIndex, uint32_t queueIndex);
  ~VulkanQueue();

  uint32_t familyIndex() const {return mFamilyIndex;}
  uint32_t queueIndex() const {return mQueueIndex;}

  void init();
  void destroy();
};


#endif /* VulkanIncludes_hpp */
