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

void HelloTriangle::initalize() {
  float vertexBuffer[] = {
     0.0,  0.8, 0.0, 1.0,
    -0.8, -0.8, 0.0, 1.0,
     0.8, -0.8, 0.0, 1.0
  };
  
  _task.frame = _graphics->getFrameBuffer("MainWindow");
  _task.frame->setToWindow(0);
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->setVertexBuffer("vertex_array", 4, 3, vertexBuffer);
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void HelloTriangle::update(float td) {
  _graphics->addTask(_task);
}
