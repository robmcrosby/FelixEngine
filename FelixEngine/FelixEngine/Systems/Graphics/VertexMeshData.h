//
//  VertexMeshData.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/25/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef VertexMeshData_h
#define VertexMeshData_h

#include <FelixEngine/GraphicResources.h>
#include <FelixEngine/StringUtils.h>
#include <vector>
#include <map>


namespace fx {
  typedef std::vector<int> IndexBuffer;
  typedef std::vector<float> VertexBuffer;
  typedef std::map<std::string, VertexBuffer> BufferMap;
  typedef std::vector<ivec2> Ranges;
  
  struct VertexMeshData {
    VERTEX_PRIMITIVE primitiveType;
    int totalVertices;
    
    IndexBuffer indices;
    Ranges      subMeshes;
    BufferMap   buffers;
    
    VertexMeshData(): primitiveType(VERTEX_TRIANGLES), totalVertices(0) {}
    bool load(const XMLTree::Node &node) {
      bool success = true;
      
      if (node.hasAttribute("type") && node.attribute("type") == "strip")
        primitiveType = VERTEX_TRIANGLE_STRIP;
      if (node.hasAttribute("total"))
        totalVertices = node.attributeAsInt("total");
      
      for (auto subNode : node)
        success &= addBuffer(*subNode);
      return success;
    }
    bool addBuffer(const XMLTree::Node &node) {
      if (node == "Buffer")
        StringUtils::parseFloats(buffers[node.attribute("name")], node.contents());
      return true;
    }
    VertexBuffer& operator[](const std::string name) {
      return buffers[name];
    }
    void addSubMesh(int start, int end) {
      subMeshes.push_back(ivec2(start, end));
    }
  };
}


#endif /* VertexMeshData_h */
