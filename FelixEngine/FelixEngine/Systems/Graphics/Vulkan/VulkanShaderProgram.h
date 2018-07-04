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
    
    int _totalVertexSets;
    int _totalFragmentSets;
    
    std::vector<VkDescriptorSetLayout> _vertexSetLayouts;
    std::vector<VkDescriptorSetLayout> _fragmentSetLayouts;
    
    static unsigned int shaderCount;
    unsigned int _shaderId;
    
  public:
    VulkanShaderProgram();
    virtual ~VulkanShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    
    virtual unsigned int getShaderId() const;
    
    void clearShaderModules();
    
    uint32_t getStageCount() const {return 2;}
    uint32_t getVertexSetCount() const {return _totalVertexSets;}
    uint32_t getFragmentSetCount() const {return _totalFragmentSets;}
    
    int getVertexLocation(const std::string &name) const;
    int getVertexUniformBinding(const std::string &name) const;
    int getFragmentUniformBinding(const std::string &name) const;
    
//    int getVertexUniformSet(const std::string &name) const;
//    int getFragmentUniformSet(const std::string &name) const;
    
    VkPipelineShaderStageCreateInfo* getStages() {return _shaderStages;}
    
    uint32_t getVertexSetLayoutCount() const {return (uint32_t)_vertexSetLayouts.size();}
    VkDescriptorSetLayout* getVertexSetLayouts();
    
    uint32_t getFragmentSetLayoutCount() const {return (uint32_t)_fragmentSetLayouts.size();}
    VkDescriptorSetLayout* getFragmentSetLayouts();
    
//    uint32_t getVertexUniformDescriptorCount(int set) const;
//    uint32_t getFragmentUniformDescriptorCount(int set) const;
//    uint32_t getFragmentTextureDescriptorCount(int set) const;
    
    std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;
    
  private:
    std::string getShaderFileName(const std::string &name) const;
    bool loadShaderModule(const std::string &fileName, SHADER_PART part);
    void reflectShaderCode(FileData &code, SHADER_PART part);
    VkDescriptorSetLayout createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> &bindingSet);
    
    bool createDescriptorSetLayouts();
  };
}

#endif /* VulkanShaderProgram_h */
