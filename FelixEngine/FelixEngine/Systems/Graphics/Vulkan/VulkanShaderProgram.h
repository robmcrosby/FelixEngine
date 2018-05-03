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


namespace fx {
  
  /**
   *
   */
  class VulkanShaderProgram: public ShaderProgram {
  private:
    
  public:
    VulkanShaderProgram();
    virtual ~VulkanShaderProgram();
    
    virtual bool loadShaderFunctions(const std::string &vertex, const std::string &fragment);
  };
}

#endif /* VulkanShaderProgram_h */
