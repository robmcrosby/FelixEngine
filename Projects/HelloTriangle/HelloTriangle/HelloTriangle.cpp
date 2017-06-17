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
  fx::Application::start();
  
  _window = _graphics->getMainWindowBuffer();
  _shader = _graphics->createShaderProgram();
  _mesh = _graphics->createVertexMesh();
}
