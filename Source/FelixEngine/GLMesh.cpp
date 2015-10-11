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

bool GLMesh::addVertexBuffer(const std::string &name, int size, int count, const float *data)
{
  return true;
}

bool GLMesh::setIndexBuffer(int count, const int *data)
{
  return true;
}

void GLMesh::setPrimitiveType(VERTEX_PRIMITIVE type)
{
}
