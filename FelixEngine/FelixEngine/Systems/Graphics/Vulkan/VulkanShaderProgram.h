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
  
  struct VertexInput {
    std::string name;
    uint32_t index;
    VkFormat format;
  };
  
  struct UniformInput {
    std::string name;
    uint32_t index;
  };
  
  /**
   *
   */
  class VulkanShaderProgram: public ShaderProgram {
  private:
    VkPipelineShaderStageCreateInfo _shaderStages[SHADER_PART_COUNT];
    std::vector<VertexInput>  _vertexInputs;
    std::vector<UniformInput> _vertexUniforms;
    std::vector<UniformInput> _fragmentUniforms;
    
  public:
    VulkanShaderProgram();
    virtual ~VulkanShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    
    void clearShaderModules();
    
    uint32_t getStageCount() const {return 2;}
    //uint32_t getVertexLocation(const std::string &name) const {return _vertexInputs.at(name);}
    VkPipelineShaderStageCreateInfo* getStages() {return _shaderStages;}
    
  private:
    std::string getShaderFileName(const std::string &name) const;
    bool loadShaderModule(const std::string &fileName, SHADER_PART part);
    void reflectShaderCode(FileData &code, SHADER_PART part);
  };
}

#endif /* VulkanShaderProgram_h */
