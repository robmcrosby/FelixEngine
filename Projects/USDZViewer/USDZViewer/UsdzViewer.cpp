//
//  UsdzViewer.cpp
//  USDZViewer
//
//  Created by Robert Crosby on 10/24/19.
//  Copyright © 2019 Robert Crosby. All rights reserved.
//

#include "UsdzViewer.h"


UsdzViewer::UsdzViewer() {
  printf("Create UsdzViewer\n");
}

UsdzViewer::~UsdzViewer() {
  
}

void UsdzViewer::initalize() {
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

void UsdzViewer::update(float td) {
  _renderPass->render();
}
