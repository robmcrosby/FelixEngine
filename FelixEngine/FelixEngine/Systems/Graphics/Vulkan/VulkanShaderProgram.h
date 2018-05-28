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
    uint32_t set;
  };
  
  struct TextureInput {
    std::string name;
    uint32_t binding;
    uint32_t set;
  };
  
  /**
   *
   */
  class VulkanShaderProgram: public ShaderProgram {
  private:
    VkPipelineShaderStageCreateInfo _shaderStages[SHADER_PART_COUNT];
    std::map<std::string, int> _vertexLocations;
    
    std::vector<UniformInput> _vertexUniforms;
    std::vector<UniformInput> _fragmentUniforms;
    std::vector<TextureInput> _fragmentTextures;
    
    int _totalVertexUniformSets;
    int _totalFragmentUniformSets;
    int _totalFragmentTextureSets;
    
    std::vector<VkDescriptorSetLayout> _vertexSetLayouts;
    std::vector<VkDescriptorSetLayout> _fragmentSetLayouts;
    
  public:
    VulkanShaderProgram();
    virtual ~VulkanShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    
    void clearShaderModules();
    
    uint32_t getStageCount() const {return 2;}
    uint32_t getVertexUniformSetCount() const {return _totalVertexUniformSets;}
    uint32_t getFragmentUniformSetCount() const {return _totalFragmentUniformSets;}
    uint32_t getFragmentTextureSetCount() const {return _totalFragmentTextureSets;}
    
    int getVertexLocation(const std::string &name) const;
    int getVertexUniformBinding(const std::string &name) const;
    int getFragmentUniformBinding(const std::string &name) const;
    
    VkPipelineShaderStageCreateInfo* getStages() {return _shaderStages;}
    
    uint32_t getVertexSetLayoutCount() const {return (uint32_t)_vertexSetLayouts.size();}
    VkDescriptorSetLayout* getVertexSetLayouts();
    
    uint32_t getFragmentSetLayoutCount() const {return (uint32_t)_fragmentSetLayouts.size();}
    VkDescriptorSetLayout* getFragmentSetLayouts();
    
    std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;
    
  private:
    std::string getShaderFileName(const std::string &name) const;
    bool loadShaderModule(const std::string &fileName, SHADER_PART part);
    void reflectShaderCode(FileData &code, SHADER_PART part);
    
    bool createDescriptorSetLayouts();
  };
}

#endif /* VulkanShaderProgram_h */
