//
//  ExampleScene.cpp
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ExampleScene.h"
#import <FelixEngine/Model.h>
#import <FelixEngine/Camera.h>

ExampleScene::ExampleScene() {
  printf("Create HelloTriangle\n");
}

ExampleScene::~ExampleScene() {
  
}

void ExampleScene::initalize() {
  float vertexBuffer[] = {
    0.0,  0.8, 0.0, 1.0,
    -0.8, -0.8, 0.0, 1.0,
    0.8, -0.8, 0.0, 1.0
  };
  
  fx::FrameBuffer *frame = _graphics->getMainWindowBuffer();
  
  fx::ShaderProgram *shader = _graphics->createShaderProgram();
  shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  fx::VertexMesh *mesh = _graphics->createVertexMesh();
  mesh->addVertexBuffer("vertex_array", 4, 3, vertexBuffer);
  
  fx::Camera *camera = _scene.getCamera("camera");
  camera->setFrame(frame);
  camera->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  
  fx::Model *model = _scene.getModel("model");
  model->setMesh(mesh);
  model->setShader(shader);
  
  _scene.renderPasses()["MainPass"].setCamera(camera);
  _scene.renderPasses()["MainPass"].addModel(model);
}

void ExampleScene::update() {
  _scene.update();
}

void ExampleScene::render() {
  _scene.render();
}
