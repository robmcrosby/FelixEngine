//
//  GLMesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLMesh.h"
#include "GLGraphicSystem.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLMesh::GLMesh(GLGraphicSystem *system): mGLSystem(system)
{
}

GLMesh::~GLMesh()
{
}

bool GLMesh::load()
{
  return true;
}

void GLMesh::setVertexBufferMap(const VertexBufferMap &map)
{
  mVertexBuffMap = map;
}

void GLMesh::addVertexBuffer(const std::string &name, int components, int count, const float *data)
{
  VertexBuffer &buffer = mVertexBuffMap[name];
  buffer.setComponents(components);
  buffer.resize(count);
  memcpy(buffer.ptr(), data, sizeof(float)*components*count);
}

void GLMesh::setIndexBuffer(int count, const int *data)
{
  IndexBuffer &buffer = mVertexBuffMap.indexBuffer();
  buffer.resize(count);
  memcpy(&buffer.at(0), data, sizeof(int)*count);
}

void GLMesh::setPrimitiveType(VERTEX_PRIMITIVE type)
{
  mVertexBuffMap.setPrimitiveType(type);
}
