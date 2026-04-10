//
//  GpuPass.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef GpuPass_hpp
#define GpuPass_hpp

#include <FelixEngine/VulkanTypes.hpp>


namespace Felix {

struct GpuPass {
  bool visible = true;
  int order = 0;
  
  VulkanRenderPassPtr pass;
  VulkanLayoutPtr     layout;
};

} /* Felix */

#endif /* GpuPass_hpp */
