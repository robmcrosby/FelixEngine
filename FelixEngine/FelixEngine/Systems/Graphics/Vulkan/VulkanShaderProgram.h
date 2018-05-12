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
#include <vulkan/vulkan.h>


namespace fx {
  
  /**
   *
   */
  class VulkanShaderProgram: public ShaderProgram {
  private:
    VkPipelineShaderStageCreateInfo _shaderStages[2];
    
  public:
    VulkanShaderProgram();
    virtual ~VulkanShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
    
    void clearShaderModules();
    
    uint32_t getStageCount() const {return 2;}
    VkPipelineShaderStageCreateInfo* getStages() {return _shaderStages;}
    
  private:
    std::string getShaderFileName(const std::string &name) const;
    VkShaderModule loadShaderModule(const std::string &fileName) const;
  };
}

#endif /* VulkanShaderProgram_h */
