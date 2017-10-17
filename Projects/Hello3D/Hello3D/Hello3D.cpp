//
//  Hello3D.cpp
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Hello3D.h"


Hello3D::Hello3D() {
  _uniformMap = std::make_shared<fx::UniformMap>();
}

Hello3D::~Hello3D() {
  
}

void Hello3D::initalize() {
  _task.frame = _graphics->getFrameBuffer("MainWindow");
  _task.frame->setToWindow(0);
  _task.frame->addDepthBuffer();
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  fx::VertexMeshData meshData;
  fx::FileSystem::loadMesh(meshData, "bunny.mesh");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->load(meshData);
  
  fx::vec2 size = fx::vec2(_task.frame->size());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _mvpUniform.view = fx::mat4::LookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  _mvpUniform.rotation = fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
  
  (*_uniformMap)["MVP"] = _mvpUniform;
  _task.uniforms.push_back(_uniformMap);
  
  _task.cullMode = fx::CULL_BACK;
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _task.setClearDepthStencil();
  
  _task.enableDepthTesting();
}

void Hello3D::update(float td) {
  // Rotate the Model
  _mvpUniform.rotation *= fx::quat::RotZ(0.02f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
  
  // Update the Model Uniforms
  (*_uniformMap)["MVP"] = _mvpUniform;
  _uniformMap->update();
  
  _graphics->addTask(_task);
}
