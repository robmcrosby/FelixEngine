//
//  ExampleScene.cpp
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ExampleScene.h"
#import <FelixEngine/Model.h>
#import <FelixEngine/StdMaterial.h>
#import <FelixEngine/Camera.h>
#import <FelixEngine/LightRig.h>
#import <FelixEngine/MeshBuilder.h>

#include <set>
#include <map>

ExampleScene::ExampleScene() {
  
}

ExampleScene::~ExampleScene() {
  
}

void ExampleScene::initalize() {
  _scene = fx::Scene::make();
  
  _renderPass = _scene->createRenderingPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->addDepthBuffer();
  _renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->setClearDepthStencil();
  
  // Set the Camera
  fx::CameraPtr camera = _scene->get<fx::Camera>("Camera");
  camera->setOrthographic(2.0f, -100.0f, 100.0f);
  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  camera->setToRenderPass(_renderPass);
  
  // Configure the Light Rig
  fx::LightRigPtr lightRig = _scene->get<fx::LightRig>("Lights");
  lightRig->addDirectionalLight(fx::vec3(-1.0f, -1.0f, 0.0f), fx::vec3(0.8f, 0.9f, 1.0f), 0.8f);
  lightRig->addPointLight(fx::vec3(1.0f, -1.0f, 2.0f), fx::vec3(0.4f, 0.4f, 1.0f), 0.4f);
  lightRig->addPointLight(fx::vec3(-4.0f, 1.0f, -6.0f), fx::vec3(1.0f, 1.0f, 1.0f), 1.0f);
  lightRig->setToRenderPass(_renderPass);
  
  // Define a Material
  fx::StdMaterialPtr material = _scene->get<fx::StdMaterial>("Material");
  material->loadShader("basic_vertex", "basic_fragment");
  material->enableDepthTesting();
  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6);
  material->setSpecularHardness(20.0);
  
  fx::MaterialPtr mat(std::static_pointer_cast<fx::Material>(material));
  
  // Setup the Bunny Model
  _model = _scene->get<fx::Model>("Bunny");
  //_model->loadMesh("Mesh.mesh");
  _model->loadMesh("Suzanne.usdz");
  _model->setMaterial(mat);
  _model->setScale(0.4f);
  _model->setRotation(fx::quat::RotX(-M_PI/2.0f) * fx::quat::RotZ(-M_PI/2.0f));
  _model->setLocation(fx::vec3(0.0f, -0.5f, 0.0f));
  _model->setFaceCulling(fx::CULL_FRONT);
  _model->setToRenderPass(_renderPass);
  
  //_modelB = _scene->get<fx::Model>("Suzanne");
  //_modelB->loadMesh("Suzanne.usdz");
  
  //_modelB = _scene->get<fx::Model>("Cube");
  //_modelB->loadMesh("Cube.usdz");
}

void ExampleScene::update(float td) {
  _model->setRotation(_model->localRotation() * fx::quat::RotZ(td));
  _scene->update(td);
}
