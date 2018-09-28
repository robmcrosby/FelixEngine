//
//  Hello3D.cpp
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Hello3D.h"


Hello3D::Hello3D() {
  
}

Hello3D::~Hello3D() {
  
}

void Hello3D::initalize() {
  fx::RenderItem renderItem;
  renderItem.loadShaderFunctions("basic_vertex", "basic_fragment");
  renderItem.loadMeshFile("bunnyMesh.mesh");
  renderItem.cullMode = fx::CULL_BACK;
  renderItem.enableDepthTesting();
  
  _renderPass = _graphics->createRenderPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->addDepthBuffer();
  _renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->setClearDepthStencil();
  _renderPass->addRenderItem(renderItem);
  
  fx::vec2 size = fx::vec2(_renderPass->getFrameSize());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _mvpUniform.view = fx::mat4::LookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  _mvpUniform.rotation = fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.6f, 0.6f, 0.6f));
  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
}

void Hello3D::update(float td) {
  // Rotate the Model
  _mvpUniform.rotation *= fx::quat::RotZ(0.02f);
  _mvpUniform.model =  _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.6f, 0.6f, 0.6f));

  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
  _renderPass->getUniformMap().update();
  
  _renderPass->render();
}
