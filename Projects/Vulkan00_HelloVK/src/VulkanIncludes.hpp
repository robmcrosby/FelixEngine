#include <iostream>
#include <string>
#include <vulkan/vulkan.h>

#ifndef VulkanIncludes_hpp
#define VulkanIncludes_hpp


typedef const std::string& StringRef;

class VulkanInstance {
private:
  VkInstance mVkInstance;

  std::string mApplicationName;
  std::string mEngineName;

  uint32_t mApplicationVersion;
  uint32_t mEngineVersion;
  uint32_t mApiVersion;

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
};


#endif /* VulkanIncludes_hpp */
