#include <VulkanTypes.hpp>

#ifndef VulkanPipeline_hpp
#define VulkanPipeline_hpp


class VulkanPipeline {
private:
  VulkanDevice*     mDevice;
  VkPipeline        mVkPipeline;
  VkPipelineLayout  mVkPipelineLayout;

  VulkanShaderPtr mKernalShader;
  VulkanShaderPtr mVertexShader;
  VulkanShaderPtr mFragmentShader;

  VkDescriptorSetLayouts mVkDescriptorSetLayouts;

public:
  VulkanPipeline(VulkanDevice* device);
  ~VulkanPipeline();

  bool setKernalShader(StringRef filename, StringRef entry = "main");
  bool setVertexShader(StringRef filename, StringRef entry = "main");
  bool setFragmentShader(StringRef filename, StringRef entry = "main");
  void clearShaders();

  bool isCompute() const {return mKernalShader != nullptr;}
  bool isGraphics() const {return mVertexShader && mFragmentShader;}

  VkPipeline getVkPipeline();
  VkPipeline getVkPipeline(VulkanLayoutSetPtr layoutSet);
  VkPipeline getVkPipeline(VkDescriptorSetLayouts setLayouts);
  VkPipeline getVkPipeline(
    VulkanRenderPassPtr renderPass,
    VulkanMeshPtr       mesh,
    VulkanLayoutSetPtr  layoutSet
  );

  VkPipelineLayout getVkPipelineLayout();
  VkPipelineLayout getVkPipelineLayout(VulkanLayoutSetPtr layoutSet);
  VkPipelineLayout getVkPipelineLayout(VkDescriptorSetLayouts setLayouts);

  void rebuild();
  void destroy();

private:
  VkPipeline createComputePipeline(VulkanLayoutSetPtr layoutSet);
  VkPipeline createComputePipeline(VkDescriptorSetLayouts setLayouts);
  VkPipeline createGraphicsPipeline(
    VulkanRenderPassPtr renderPass,
    VulkanMeshPtr       mesh,
    VulkanLayoutSetPtr  layoutSet
  );

  VkDescriptorSetLayouts& getVkDescriptorSetLayouts();
  void clearVkDescriptorSetLayouts();

  void getLayoutBindingSets(LayoutBindingSets& layoutBindingSets);

  VkPipelineLayout createVkPipelineLayout(const VkDescriptorSetLayouts& setLayouts);

  void destroyPipelineLayout();
  void destroyPipeline();
};


#endif /* VulkanPipeline_hpp */
