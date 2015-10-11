//
//  GLMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLMesh_h
#define GLMesh_h

#include "GLGraphicSystem.h"
#include "GLShader.h"
#include <vector>
#include <map>

namespace fx
{
  /**
   *
   */
  class GLMesh: public Mesh
  {
  public:
    GLMesh(GLGraphicSystem *system);
    virtual ~GLMesh();
    
    virtual void reload();
    
    bool load();
    void draw(const GLShader *shader, int instances, int index) const
    {
    }
    
  private:
    struct GLRange
    {
      GLRange(): start(0), end(0) {}
      GLuint start, end;
    };
    typedef std::vector<GLRange> GLRanges;
    
    struct GLBuffer
    {
      GLBuffer(): components(0), count(0), bufferId(0) {}
      GLuint components, count, bufferId;
    };
    typedef std::map<std::string, GLBuffer> GLBufferMap;
    
  private:
    void clearBuffers();
    bool loadVertexBuffer(const std::string &name, const VertexBuffer &buffer);
    bool loadIndexBuffer(const IndexBuffer &buffer);
    void setGLPrimitiveType(VERTEX_PRIMITIVE type);
    
    GLGraphicSystem *mGLSystem;
    
    GLBufferMap mVertexBuffers;
    GLuint      mIndexBuffer;
    GLenum      mPrimitiveType;
    GLRanges    mSubMeshes;
  };
}

#endif /* GLMesh_h */
