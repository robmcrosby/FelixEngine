//
//  GLMesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLMesh.h"
#include "GLGraphicSystem.h"


using namespace fx;
using namespace std;


GLMesh::GLMesh(GLGraphicSystem *system): mGLSystem(system), mIndexBuffer(0)
{
}

GLMesh::~GLMesh()
{
  clearBuffers();
}

void GLMesh::reload()
{
  GraphicResource::reload();
  if (mGLSystem)
    mGLSystem->loadOnNextUpdate();
}

bool GLMesh::load()
{
  if (!mLoaded)
  {
    mLoaded = true;
    clearBuffers();
    
    for (VertexBufferMap::const_iterator buffer = mBufferMap.begin(); buffer != mBufferMap.end(); ++buffer)
      mLoaded &= loadVertexBuffer(buffer->first, buffer->second);
    if (mBufferMap.indexBuffer().size())
      mLoaded &= loadIndexBuffer(mBufferMap.indexBuffer());
    
    const Ranges &subMeshes = mBufferMap.subMeshes();
    if (subMeshes.size())
    {
      for (Ranges::const_iterator range = subMeshes.begin(); range != subMeshes.end(); ++range)
      {
        mSubMeshes.push_back(GLRange());
        mSubMeshes.back().start = range->start;
        mSubMeshes.back().end = range->end;
      }
    }
    else
    {
      mSubMeshes.push_back(GLRange());
      mSubMeshes.back().end = mBufferMap.count();
    }
    setGLPrimitiveType(mBufferMap.primitiveType());
  }
  return mLoaded;
}

void GLMesh::addVertexBuffer(const std::string &name, int components, int count, const float *data)
{
  VertexBuffer &buffer = mBufferMap[name];
  buffer.setComponents(components);
  buffer.resize(count);
  memcpy(buffer.ptr(), data, sizeof(float)*components*count);
  reload();
}

void GLMesh::setIndexBuffer(int count, const int *data)
{
  IndexBuffer &buffer = mBufferMap.indexBuffer();
  buffer.resize(count);
  memcpy(&buffer.at(0), data, sizeof(int)*count);
  reload();
}

void GLMesh::setPrimitiveType(VERTEX_PRIMITIVE type)
{
  mBufferMap.setPrimitiveType(type);
  reload();
}

void GLMesh::clearBuffers()
{
  // Clear the Vertex Buffers
  for (GLBufferMap::iterator buffer = mVertexBuffers.begin(); buffer != mVertexBuffers.end(); ++buffer)
    glDeleteBuffers(1, &buffer->second.bufferId);
  mVertexBuffers.clear();
  
  // Clear the Index Buffer
  if (mIndexBuffer)
  {
    glDeleteBuffers(1, &mIndexBuffer);
    mIndexBuffer = 0;
  }
  mSubMeshes.clear();
}

bool GLMesh::loadVertexBuffer(const string &name, const VertexBuffer &buffer)
{
  // Create the Vertex Buffer
  GLBuffer glBuffer;
  glGenBuffers(1, &glBuffer.bufferId);
  if (!glBuffer.bufferId)
  {
    cerr << "Error Creating Vertex Buffer." << endl;
    return false;
  }
  glBuffer.components = buffer.components();
  glBuffer.count = buffer.count();
  
  // Load the Vertices
  glBindBuffer(GL_ARRAY_BUFFER, glBuffer.bufferId);
  GLsizeiptr bufSize = glBuffer.components * glBuffer.count * sizeof(float);
  const GLvoid *ptr = (const GLvoid*)buffer.ptr();
  glBufferData(GL_ARRAY_BUFFER, bufSize, ptr, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  mVertexBuffers[name] = glBuffer;
  return true;
}

bool GLMesh::loadIndexBuffer(const IndexBuffer &buffer)
{
  // Create the Index Buffer
  glGenBuffers(1, &mIndexBuffer);
  if (!mIndexBuffer)
  {
    cerr << "Error Creating Index Buffer." << endl;
    return false;
  }
  
  // Load the Indices.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
  GLsizeiptr bufSize = buffer.size() * sizeof(unsigned int);
  const GLvoid *ptr = (const GLvoid*)&buffer.at(0);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,  bufSize, ptr, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  return true;
}

void GLMesh::setGLPrimitiveType(VERTEX_PRIMITIVE type)
{
  if (type == VERTEX_TRIANGLES)
    mPrimitiveType = GL_TRIANGLES;
  if (type == VERTEX_TRIANGLE_STRIP)
    mPrimitiveType = GL_TRIANGLE_STRIP;
}
