//
//  VulkanVertexMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanVertexMesh_h
#define VulkanVertexMesh_h

#include "GraphicResources.h"


namespace fx {
  
  /**
   *
   */
  class VulkanVertexMesh: public VertexMesh {
  private:
    
  public:
    VulkanVertexMesh();
    virtual ~VulkanVertexMesh();
    
    virtual bool load(const VertexMeshData &data);
    virtual void setPrimativeType(VERTEX_PRIMITIVE type);
    virtual bool setIndexBuffer(size_t count, const int *buffer);
    virtual bool setVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer);
  };
}

#endif /* VulkanVertexMesh_h */
