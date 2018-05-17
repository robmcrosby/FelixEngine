//
//  VulkanShaderProgram.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanShaderProgram_h
#define VulkanShaderProgram_h

#include "GraphicResources.h"
#include "FileSystem.h"
#include <vulkan/vulkan.h>


namespace fx {
  
  struct UniformInput {
    std::string name;
    uint32_t binding;
  };
  
  /**
   *
   */
  class VulkanShaderProgram: public ShaderProgram {
  private:
    VkPipelineShaderStageCreateInfo _shaderStages[SHADER_PART_COUNT];
    std::map<std::string, int> _vertexLocations;
    std::vector<UniformInput>  _vertexUniforms;
    std::vector<UniformInput>  _fragmentUniforms;
    
    std::vector<VkDescriptorSetLayout> _descriptorSetLayouts;
    
  public:
    VulkanShaderProgram();
    virtual ~VulkanShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    
    void clearShaderModules();
    
    uint32_t getStageCount() const {return 2;}
    int getVertexLocation(const std::string &name) const;
    VkPipelineShaderStageCreateInfo* getStages() {return _shaderStages;}
    
    uint32_t getDescriptorSetLayoutCount() const {return (uint32_t)_descriptorSetLayouts.size();}
    VkDescriptorSetLayout* getDescriptorSetLayouts();
    
  private:
    std::string getShaderFileName(const std::string &name) const;
    bool loadShaderModule(const std::string &fileName, SHADER_PART part);
    void reflectShaderCode(FileData &code, SHADER_PART part);
    
    bool createDescriptorSetLayouts();
  };
}

#endif /* VulkanShaderProgram_h */
