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
     0.0,  0.8, 0.0,
    -0.8, -0.8, 0.0,
     0.8, -0.8, 0.0
  };
  
  fx::RenderItem renderItem;
  renderItem.loadShaderFunctions("basic_vertex", "basic_fragment");
  renderItem.setMeshVertexBuffer("vertex_array", 3, 3, vertexBuffer);
  renderItem.loadMesh();

  _renderPass = _graphics->createRenderPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->addRenderItem(renderItem);
}

void HelloTriangle::update(float td) {
  _renderPass->render();
}
