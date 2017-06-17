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
  float vertexBuffer[] = {0.0,  0.8, 0.0, -0.8, -0.8, 0.0, 0.8, -0.8, 0.0};
  
  fx::Application::start();
  
  _window = _graphics->getMainWindowBuffer();
  
  _shader = _graphics->createShaderProgram();
  _shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  _mesh = _graphics->createVertexMesh();
  _mesh->addVertexBuffer(3, 1, vertexBuffer);
}
