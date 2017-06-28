//
//  Hello3D.cpp
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Hello3D.h"


Hello3D::Hello3D() {
  printf("Create Hello3D\n");
}

Hello3D::~Hello3D() {
  
}

void Hello3D::start() {
  _task.frame = _graphics->getMainWindowBuffer();
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  fx::VertexMeshData meshData;
  fx::FileSystem::loadMesh(meshData, "bunny.mesh");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->loadData(meshData);
  
  fx::vec2 size = fx::vec2(_task.frame->size());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _mvpUniform.view = fx::mat4::LookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  _mvpUniform.rotation = fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
  
  _task.uniforms["MVP"] = _graphics->createUniformBuffer();
  _task.uniforms["MVP"]->load(&_mvpUniform, sizeof(MVPUniform));
  
  _task.cullMode = fx::CULL_BACK;
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _task.setClearDepthStencil();
  
  _task.depthState.enableTesting();
  _task.depthState.enableWriting();
  _task.depthState.setFunction(fx::DEPTH_TEST_LESS);
}

void Hello3D::update() {
  _graphics->nextFrame();
  _graphics->addTask(_task);
  _graphics->render();
}

