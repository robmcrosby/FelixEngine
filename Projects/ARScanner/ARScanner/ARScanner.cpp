//
//  ARScanner.cpp
//  ARScanner
//
//  Created by Robert Crosby on 9/28/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "ARScanner.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>


ARScanner::ARScanner() {
  _transform = fx::Transform::make();
}

ARScanner::~ARScanner() {
  
}

void ARScanner::initalize() {
  _scene.loadXMLFile("Scene.xml");
  fx::ModelPtr model = _scene.get<fx::Model>("Model");
  model->transform()->setParrent(_transform);
  
//  setupFirstPass();
//  setupSecondPass();
}

void ARScanner::update(float td) {
  _transform->setRotation(_transform->localRotation() * fx::quat::RotY(td));
  _scene.update(td);
  
//  // Rotate the Model
//  _firstUniform.rotation *= fx::quat::RotZ(td);
//  _firstUniform.model = _firstUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
//
//  // Update the Model Uniforms
//  _firstRenderPass->getUniformMap()["MVP"] = _firstUniform;
//  _firstRenderPass->getUniformMap().update();
//
//  _firstRenderPass->render();
//  _secondRenderPass->render();
}

//void ARScanner::setupFirstPass() {
//  fx::RenderItem renderItem;
//  renderItem.loadShaderFunctions("basic_vertex", "basic_fragment");
//  renderItem.loadMeshFile("bunny.mesh");
//  renderItem.cullMode = fx::CULL_BACK;
//  renderItem.enableDepthTesting();
//
//  _firstRenderPass = _graphics->createRenderPass();
//  _firstRenderPass->addRenderItem(renderItem);
//  _firstRenderPass->setClearColor(fx::vec4(0.2f, 0.2f, 0.2f, 1.0f));
//  _firstRenderPass->setClearDepthStencil();
//  _firstRenderPass->resizeFrame(1024, 1024);
//  _firstRenderPass->addColorTexture();
//  _firstRenderPass->addDepthBuffer();
//
//  fx::vec2 size = fx::vec2(_firstRenderPass->getFrameSize());
//  float width = 2.0f;
//  float height = 2.0f * size.h/size.w;
//  _firstUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
//  _firstUniform.view = fx::mat4::LookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
//  _firstUniform.rotation = fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f);
//  _firstUniform.model = _firstUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
//  _firstRenderPass->getUniformMap()["MVP"] = _firstUniform;
//}

//void ARScanner::setupSecondPass() {
//  float vertexBuffer[] = {
//    -1.0f, -1.0f, 0.0f, 1.0f,
//    -1.0f,  1.0f, 0.0f, 1.0f,
//    1.0f, -1.0f, 0.0f, 1.0f,
//    1.0f,  1.0f, 0.0f, 1.0f
//  };
//
//  float uvBuffer[] = {
//    0.0f, 1.0f,
//    0.0f, 0.0f,
//    1.0f, 1.0f,
//    1.0f, 0.0f
//  };
//
//  fx::RenderItem renderItem;
//  renderItem.loadShaderFunctions("texture_vertex", "texture_fragment");
//  renderItem.setMeshVertexBuffer("Position", 4, 4, vertexBuffer);
//  renderItem.setMeshVertexBuffer("UV", 2, 4, uvBuffer);
//  renderItem.setMeshPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
//  renderItem.loadMesh();
//  renderItem.addTexture(_firstRenderPass->getColorTexture());
//
//  _secondRenderPass = _graphics->createRenderPass();
//  _secondRenderPass->addRenderItem(renderItem);
//  _secondRenderPass->setFrameToWindow(0);
//  _secondRenderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
//
//  fx::vec2 size = fx::vec2(_secondRenderPass->getFrameSize());
//  float width = 2.0f;
//  float height = 2.0f * size.h/size.w;
//  _secondUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
//  _secondUniform.view = fx::mat4();
//  _secondUniform.model = fx::mat4::Scale(fx::vec3(0.8f, 0.8f, 0.8f));
//  _secondRenderPass->getUniformMap()["MVP"] = _secondUniform;
//
//}
