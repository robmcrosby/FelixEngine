//
//  Hello3D.cpp
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Hello3D.h"
#include <FelixEngine/Variant.h>

Hello3D::Hello3D() {
  printf("Create Hello3D\n");
}

Hello3D::~Hello3D() {
  
}

void Hello3D::start() {
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
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void Hello3D::update() {
  _graphics->nextFrame();
  _graphics->addTask(_task);
  _graphics->render();
}

