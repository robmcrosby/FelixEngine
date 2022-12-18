#include <VulkanTypes.hpp>

#ifndef VulkanShader_hpp
#define VulkanShader_hpp


class VulkanShader {
private:
  VulkanDevice*         mDevice;
  VkShaderModule        mVkShaderModule;
  std::string           mEntryFunction;
  LayoutBindingSets     mLayoutBindingSets;
  VkShaderStageFlagBits mVkShaderStage;

public:
  VulkanShader(VulkanDevice* device, VkShaderStageFlagBits stage);
  ~VulkanShader();

  bool load(StringRef filename, StringRef entry = "main");
  bool load(SPIRVCodeRef code, StringRef entry = "main");

  VkShaderModule getVkShaderModule() const {return mVkShaderModule;}
  CString getEntryFunction() const {return mEntryFunction.c_str();}
  LayoutBindingSets& getLayoutBindingSets() {return mLayoutBindingSets;}

  VkPipelineShaderStageCreateInfo getVkPipelineShaderStageCreateInfo();

  void destroy();

private:
  void reflect(SPIRVCodeRef code, StringRef entry);
  static bool readSPIRV(SPIRVCode& code, StringRef filename);
};


#endif /* VulkanShader_hpp */
