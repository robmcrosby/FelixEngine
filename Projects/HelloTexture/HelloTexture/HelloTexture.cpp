//
//  HelloTexture.cpp
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "HelloTexture.h"

HelloTexture::HelloTexture() {
  printf("Create HelloTexture\n");
}

HelloTexture::~HelloTexture() {
  
}

void HelloTexture::initalize() {
  float vertexBuffer[] = {
    0.0,  0.8, 0.0, 1.0,
    -0.8, -0.8, 0.0, 1.0,
    0.8, -0.8, 0.0, 1.0
  };
  
  _task.frame = _graphics->getMainWindowBuffer();
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->addVertexBuffer("vertex_array", 4, 3, vertexBuffer);
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void HelloTexture::render() {
  _graphics->addTask(_task);
}
