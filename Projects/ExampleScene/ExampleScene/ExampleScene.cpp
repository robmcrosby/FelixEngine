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
#import <FelixEngine/MeshBuilder.h>

#include <set>
#include <map>

ExampleScene::ExampleScene() {
  
}

ExampleScene::~ExampleScene() {
  
}

void ExampleScene::initalize() {
  _scene = fx::Scene::make();
  
  _renderPass = _graphics->createRenderPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->addDepthBuffer();
  _renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->setClearDepthStencil();
  
  fx::CameraPtr camera = _scene->get<fx::Camera>("Camera");
  camera->setToRenderPass(_renderPass);
  camera->setOrthographic(2.0f, -100.0f, 100.0f);
  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  
  fx::LightRigPtr lightRig = _scene->get<fx::LightRig>("Lights");
  lightRig->setToRenderPass(_renderPass);
  lightRig->addDirectionalLight(fx::vec3(-1.0f, -1.0f, 0.0f), fx::vec3(0.8f, 0.9f, 1.0f), 0.8f);
  lightRig->addPointLight(fx::vec3(1.0f, -1.0f, 2.0f), fx::vec3(0.4f, 0.4f, 1.0f), 0.4f);
  lightRig->addPointLight(fx::vec3(-4.0f, 1.0f, -6.0f), fx::vec3(1.0f, 1.0f, 1.0f), 1.0f);
  
  
//  fx::FrameBufferPtr frame = _graphics->getFrameBuffer("MainWindow");
//  frame->setToWindow(0);
//  frame->addDepthBuffer();
//
//  // Setup the Camera
//  fx::CameraPtr camera = _scene.get<fx::Camera>("Camera");
//  camera->setFrame(frame);
//  camera->setOrthographic(2.0f, -100.0f, 100.0f);
//  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
//  camera->setClearColor(fx::vec4(0.2f, 0.2f, 0.2f, 1.0f));
//  camera->setClearDepth();
//  camera->addToRenderPass("MainPass");
//
//  // Setup the Lights
//  fx::LightPtr lightA = _scene.get<fx::Light>("LightA");
//  lightA->setAsDirectionalLight(fx::vec3(-1.0f, -1.0f, 0.0f), fx::vec3(0.8f, 0.9f, 1.0f), 0.8f);
//  lightA->addToRenderPass("MainPass");
//
//  fx::LightPtr lightB = _scene.get<fx::Light>("LightB");
//  lightB->setAsPointLight(fx::vec3(1.0f, -1.0f, 2.0f), fx::vec3(0.4f, 0.4f, 1.0f), 0.4f);
//  lightB->addToRenderPass("MainPass");
//
//  fx::LightPtr lightC = _scene.get<fx::Light>("LightC");
//  lightC->setAsPointLight(fx::vec3(-4.0f, 1.0f, -6.0f), fx::vec3(1.0f, 1.0f, 1.0f), 1.0f);
//  lightC->addToRenderPass("MainPass");
//
//  // Load the Shader
//  fx::ShaderProgramPtr shader = _graphics->createShaderProgram();
//  shader->loadShaderFunctions("basic_vertex", "basic_fragment");
//
//  // Load the Bunny Mesh
//  fx::VertexMeshPtr mesh = MeshBuilder::createFromFile("Mesh.mesh");
//  //fx::VertexMeshPtr mesh = MeshBuilder::createCube();
//
//  // Setup the Material
//  fx::MaterialPtr material = _scene.get<fx::Material>("Material");
//  material->setShader(shader);
//  material->enableDepthTesting();
//  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
//  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
//  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6);
//  material->setSpecularHardness(20.0);
//
//  // Setup the Bunny Model
//  _model = _scene.get<fx::Model>("Bunny");
//  _model->setMesh(mesh);
//  _model->setMaterial(material);
//  _model->setScale(0.4f);
//  _model->setRotation(fx::quat::RotX(-M_PI/2.0f) * fx::quat::RotZ(-M_PI/2.0f));
//  _model->setLocation(fx::vec3(0.0f, -0.5f, 0.0f));
//  _model->addToRenderPass("MainPass");
//  //_model->setFaceCulling(fx::CULL_BACK);
//
//  // Setup the Render Scheme
//  _renderScheme.push_back("MainPass");
}

void ExampleScene::update(float td) {
//  _model->setRotation(_model->localRotation() * fx::quat::RotZ(td));
  _scene->update(td);
  
  _renderPass->render();
  _renderPass->clearRenderItems();
//
//  fx::RenderPass::renderPasses(_renderScheme);
//  fx::RenderPass::resetPasses();
}
