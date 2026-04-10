//
//  GpuDraw.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef GpuDraw_hpp
#define GpuDraw_hpp

#include <FelixEngine/VulkanTypes.hpp>


namespace Felix {

struct GpuDraw {
  bool visible = true;
  int order = 0;
  uint32_t instances = 1;
  
  VulkanMeshPtr      mesh;
  VulkanPipelinePtr  pipeline;
  VulkanLayoutSetPtr layoutSet;
};

} /* Felix */

#endif /* GpuDraw_hpp */
