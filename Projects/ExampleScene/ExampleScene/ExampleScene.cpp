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
#import <FelixEngine/Light.h>

#include <set>
#include <map>

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
  
  _task.frame = _graphics->getMainWindowBuffer();
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("tri_vertex", "tri_fragment");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->addVertexBuffer("vertex_array", 4, 3, vertexBuffer);
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  
  
  // Setup the Camera
  fx::CameraPtr camera = _scene.get<fx::Camera>("Camera");
  camera->addDepthBuffer();
  camera->setOrthographic(2.0f, -100.0f, 100.0f);
  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  camera->setClearColor(fx::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  camera->setClearDepth();
  camera->addToRenderPass("main");
  
  // Setup the Lights
  fx::LightPtr lightA = _scene.get<fx::Light>("LightA");
  lightA->setAsDirectionalLight(fx::vec3(-1.0f, -1.0f, 0.0f), fx::vec3(0.8f, 0.9f, 1.0f), 0.8f);
  lightA->addToRenderPass("main");
  
  fx::LightPtr lightB = _scene.get<fx::Light>("LightB");
  lightB->setAsPointLight(fx::vec3(1.0f, -1.0f, 2.0f), fx::vec3(0.4f, 0.4f, 1.0f), 0.4f);
  lightB->addToRenderPass("main");
  
  fx::LightPtr lightC = _scene.get<fx::Light>("LightC");
  lightC->setAsPointLight(fx::vec3(-4.0f, 1.0f, -6.0f), fx::vec3(1.0f, 1.0f, 1.0f), 1.0f);
  lightC->addToRenderPass("main");
  
  // Load the Shader
  fx::ShaderPtr shader = _graphics->createShaderProgram();
  shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  // Load the Bunny Mesh
  fx::VertexMeshData meshData;
  fx::FileSystem::loadMesh(meshData, "bunny.mesh");
  fx::VertexPtr mesh = _graphics->createVertexMesh();
  mesh->load(meshData);
  
  // Setup the Material
  fx::MaterialPtr material = _scene.get<fx::Material>("Material");
  material->setShader(shader);
  material->enableDepthTesting();
  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6, 20.0);
  
  // Setup the Bunny Model
  _model = _scene.get<fx::Model>("Bunny");
  _model->setMesh(mesh);
  _model->setMaterial(material);
  _model->setScale(0.2f);
  _model->setOrientation(fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f));
  _model->addToRenderPass("main");

  
//  fx::ShaderPtr shader = _graphics->createShaderProgram();
//  shader->loadShaderFunctions("basic_vertex", "basic_fragment");
//
//  fx::VertexMeshData meshData;
//  fx::FileSystem::loadMesh(meshData, "bunny.mesh");
//
//  fx::VertexPtr mesh = _graphics->createVertexMesh();
//  mesh->load(meshData);
//
//  fx::Camera *camera = _scene.getCamera("camera");
//  camera->addDepthBuffer();
//  camera->setOrthographic(2.0f, -100.0f, 100.0f);
//  camera->lookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
//  camera->setClearColor(fx::vec4(0.2f, 0.2f, 0.2f, 1.0f));
//  camera->setClearDepth();
//
//  fx::LightRig *lightRig = _scene.getLightRig("Lights");
//  lightRig->addDirectionalLight(fx::vec3(-1.0f, -1.0f, 0.0f), fx::vec3(0.8f, 0.9f, 1.0f), 0.8f);
//  lightRig->addPointLight(fx::vec3(1.0f, -1.0f, 2.0f), fx::vec3(0.4f, 0.4f, 1.0f), 0.4f);
//  lightRig->addPointLight(fx::vec3(-4.0f, 1.0f, -6.0f), fx::vec3(1.0f, 1.0f, 1.0f), 1.0f);
//  camera->setLightRig(lightRig);
//
//  fx::Material *material = _scene.getMaterial("Material");
//  material->setShader(shader);
//  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
//  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
//  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6, 20.0);
//
//  _model = _scene.getModel("model");
//  _model->setMesh(mesh);
//  _model->setMaterial(material);
//  _model->enableDepthTesting();
//  _model->setScale(0.2f);
//  _model->setOrientation(fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f));
//  _model->update();
//
//  _scene.renderPasses()["MainPass"].setCamera(camera);
//  _scene.renderPasses()["MainPass"].addModel(_model);
}

void ExampleScene::update(float td) {
  _model->setOrientation(_model->orientation() * fx::quat::RotZ(td));
  _scene.update(td);
}

void ExampleScene::render() {
  _graphics->addTask(_task);
  
 // _scene.render();
}
