//
//  MeshLoader.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef MeshLoader_h
#define MeshLoader_h

#include "VertexBufferMap.h"
#include "BufferMap.h"
#include "XMLTree.h"


namespace fx
{
  class MeshLoader
  {
  public:
    static bool LoadMeshFromXML(VertexBufferMap &bufferMap, const XMLTree::Node &node);
    static bool LoadMeshFromStream(VertexBufferMap &bufferMap, std::istream &is);
    
    static bool LoadMeshFromFile(VertexBufferMap &bufferMap, const std::string &file);
    
    static bool LoadMeshPrimitive(VertexBufferMap &bufferMap, const XMLTree::Node &node);
    static bool LoadMeshPlane(VertexBufferMap &bufferMap, const vec2 &size, const vec2 &offset);
    
    
    
    static bool LoadMeshFromXML(BufferMap &bufferMap, const XMLTree::Node &node);
    static bool LoadMeshFromStream(BufferMap &bufferMap, std::istream &is);
    
    static bool LoadMeshFromFile(BufferMap &bufferMap, const std::string &file);
    
    static bool LoadMeshPrimitive(BufferMap &bufferMap, const XMLTree::Node &node);
    static bool LoadMeshPlane(BufferMap &bufferMap, const vec2 &size, const vec2 &offset);
    
  private:
    static bool AddBuffer(VertexBufferMap &bufferMap, const XMLTree::Node &node);
    static bool AddSubMesh(VertexBufferMap &bufferMap, const XMLTree::Node &node);
    
    static bool ReadBufferFromStream(BufferMap &bufferMap, int numVertices, std::istream &is);
    
    
    
    static bool AddBuffer(BufferMap &bufferMap, const XMLTree::Node &node);
    static bool AddIndices(BufferMap &bufferMap, const XMLTree::Node &node);
    static bool AddSubMesh(Buffer &buffer, const XMLTree::Node &node);

    
    static VERTEX_PRIMITIVE GetVertexPrimitive(int value);
    
    static bool ReadBufferFromStream(VertexBufferMap &bufferMap, int numVertices, std::istream &is);
  };
}

#endif /* MeshLoader_h */
