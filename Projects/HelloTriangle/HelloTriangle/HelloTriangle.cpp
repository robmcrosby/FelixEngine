//
//  HelloTriangle.cpp
//  HelloTriangle
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "HelloTriangle.h"

HelloTriangle::HelloTriangle() {
  printf("Create HelloTriangle\n");
}

HelloTriangle::~HelloTriangle() {
  
}

void HelloTriangle::start() {
  float vertexBuffer[] = {
     0.0,  0.8, 0.0, 1.0,
    -0.8, -0.8, 0.0, 1.0,
     0.8, -0.8, 0.0, 1.0
  };
  
  _task.frame = _graphics->getMainWindowBuffer();
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->addVertexBuffer(4, 3, vertexBuffer);
}

void HelloTriangle::update() {
  _graphics->nextFrame();
  _graphics->addTask(_task);
  _graphics->render();
}
