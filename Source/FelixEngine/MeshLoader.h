//
//  MeshLoader.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef MeshLoader_h
#define MeshLoader_h

#include "XMLTree.h"
#include "GraphicResources.h"


namespace fx
{
  class Buffer;
  
  class MeshLoader
  {
  public:

    static bool LoadMeshFromXML(Buffer &bufferMap, const XMLTree::Node &node);
    static bool LoadMeshFromStream(Buffer &bufferMap, std::istream &is);
    
    static bool LoadMeshFromFile(Buffer &bufferMap, const std::string &file);
    
    static bool LoadMeshPrimitive(Buffer &bufferMap, const XMLTree::Node &node);
    static bool LoadMeshPlane(Buffer &bufferMap, const vec2 &size, const vec2 &offset);
    
  private:
    static bool ReadBufferFromStream(Buffer &bufferMap, int numVertices, std::istream &is);
    
    static bool AddBuffer(Buffer &bufferMap, const XMLTree::Node &node);
    static bool AddIndices(Buffer &bufferMap, const XMLTree::Node &node);
    static bool AddSubMesh(Buffer &buffer, const XMLTree::Node &node);

    static VERTEX_PRIMITIVE GetVertexPrimitive(int value);
  };
}

#endif /* MeshLoader_h */
