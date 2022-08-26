#include <iostream>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#ifndef VulkanIncludes_hpp
#define VulkanIncludes_hpp


typedef const char* CString;
typedef std::vector<CString> CStrings;
typedef std::vector<std::string> Strings;
typedef const std::string& StringRef;

class VulkanInstance {
private:
  VkInstance mVkInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;

  std::string mApplicationName;
  std::string mEngineName;

  uint32_t mApplicationVersion;
  uint32_t mEngineVersion;
  uint32_t mApiVersion;

  bool mValidationEnabled;
  CStrings mValidationLayers;
  CStrings mExtensions;

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
  void addValidationLayer(CString layer);
  void addExtension(CString extension);

  void getAvailableLayers(std::vector<VkLayerProperties> &layers) const;
  void getAvailableExtensions(std::vector<VkExtensionProperties> &extensions) const;

  friend std::ostream &operator<<(std::ostream &os, const VulkanInstance &instance);

private:
  bool createVkInstance();

  bool setupDebugMessenger();
  void destroyDebugMessenger();
  void initDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  bool loadDevices();
  void clearDevices();
};


#endif /* VulkanIncludes_hpp */
