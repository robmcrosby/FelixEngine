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
#import <FelixEngine/LightRig.h>

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
  camera->setOrthographic(2.0f, -100.0f, 100.0f);
  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  camera->setClearColor(fx::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  camera->setClearDepth();
  
  fx::LightRig *lightRig = _scene.getLightRig("Lights");
  lightRig->addDirectionalLight(fx::vec3(-1.0f, -1.0f, 0.0f), fx::vec3(0.8f, 0.9f, 1.0f), 0.8f);
  lightRig->addPointLight(fx::vec3(1.0f, -1.0f, 2.0f), fx::vec3(0.4f, 0.4f, 1.0f), 0.4f);
  lightRig->addPointLight(fx::vec3(-4.0f, 1.0f, -6.0f), fx::vec3(1.0f, 1.0f, 1.0f), 1.0f);
  camera->setLightRig(lightRig);
  
  fx::Material *material = _scene.getMaterial("Material");
  material->setShader(shader);
  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6, 20.0);
  
  _model = _scene.getModel("model");
  _model->setMesh(mesh);
  _model->setMaterial(material);
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
