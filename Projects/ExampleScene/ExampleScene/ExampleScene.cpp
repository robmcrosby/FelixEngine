//
//  ExampleScene.cpp
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ExampleScene.h"
#import <FelixEngine/Model.h>
#import <FelixEngine/Material.h>
#import <FelixEngine/Camera.h>

ExampleScene::ExampleScene() {
  printf("Create HelloTriangle\n");
}

ExampleScene::~ExampleScene() {
  
}

void ExampleScene::initalize() {
  fx::FrameBuffer *frame = _graphics->getMainWindowBuffer();
  frame->addDepthBuffer();
  
  fx::ShaderProgram *shader = _graphics->createShaderProgram();
  shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  fx::VertexMeshData meshData;
  fx::FileSystem::loadMesh(meshData, "bunny.mesh");
  
  fx::VertexMesh *mesh = _graphics->createVertexMesh();
  mesh->load(meshData);
  
  fx::Camera *camera = _scene.getCamera("camera");
  camera->setFrame(frame);
  camera->setOrthographic(2.0f, -100.0f, 100.0f);
  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  camera->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  camera->setClearDepth();
  
  _model = _scene.getModel("model");
  _model->setMesh(mesh);
  _model->setShader(shader);
  _model->enableDepthTesting();
  _model->setScale(0.2f);
  _model->setOrientation(fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f));
  _model->update();
  
  _scene.renderPasses()["MainPass"].setCamera(camera);
  _scene.renderPasses()["MainPass"].addModel(_model);
}

void ExampleScene::update() {
  _model->setOrientation(_model->orientation() * fx::quat::RotZ(0.02f));
  _model->update();
  
  _scene.update();
}

void ExampleScene::render() {
  _scene.render();
}
