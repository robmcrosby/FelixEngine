//
//  HelloFBO.cpp
//  HelloFBO
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "HelloFBO.h"


HelloFBO::HelloFBO() {
  _firstUniformMap = std::make_shared<fx::UniformMap>();
  _secondUniformMap = std::make_shared<fx::UniformMap>();
  _secondTextureMap = std::make_shared<fx::TextureMap>();
}

HelloFBO::~HelloFBO() {
  
}

void HelloFBO::initalize() {
  setupFirstTask();
  setupSecondTask();
}

void HelloFBO::update(float td) {
  // Rotate the Model
  _firstUniform.rotation *= fx::quat::RotZ(td);
  _firstUniform.model = _firstUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
  
  // Update the Model Uniforms
  (*_firstUniformMap)["MVP"] = _firstUniform;
  _firstUniformMap->update();
  
  _graphics->addTask(_firstTask);
  _graphics->addTask(_secondTask);
}

void HelloFBO::setupFirstTask() {
  _firstTask.frame = _graphics->createFrameBuffer();
  _firstTask.frame->resize(1024, 1024);
  _firstTask.frame->addColorTexture();
  _firstTask.frame->addDepthBuffer();
  
  _firstTask.shader = _graphics->createShaderProgram();
  _firstTask.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  fx::VertexMeshData meshData;
  fx::FileSystem::loadMesh(meshData, "bunny.mesh");
  
  _firstTask.mesh = _graphics->createVertexMesh();
  _firstTask.mesh->load(meshData);
  
  fx::vec2 size = fx::vec2(_firstTask.frame->size());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _firstUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _firstUniform.view = fx::mat4::LookAt(fx::vec3(10.0f, 10.0f, 10.0f), fx::vec3(0.0f, 0.0f, 0.0f), fx::vec3(0.0f, 1.0f, 0.0f));
  _firstUniform.rotation = fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f);
  _firstUniform.model = _firstUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.2f, 0.2f, 0.2f));
  (*_firstUniformMap)["MVP"] = _firstUniform;
  _firstTask.uniforms.push_back(_firstUniformMap);
  
  _firstTask.cullMode = fx::CULL_BACK;
  
  _firstTask.setClearColor(fx::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  _firstTask.setClearDepthStencil();
  
  _firstTask.enableDepthTesting();
}

void HelloFBO::setupSecondTask() {
  float vertexBuffer[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, 0.0f, 1.0f
  };
  
  float uvBuffer[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  
  _secondTask.frame = _graphics->getMainWindowBuffer();
  
  _secondTask.shader = _graphics->createShaderProgram();
  _secondTask.shader->loadShaderFunctions("basic_uv", "basic_texture");
  
  _secondTask.mesh = _graphics->createVertexMesh();
  _secondTask.mesh->setVertexBuffer("Position", 4, 4, vertexBuffer);
  _secondTask.mesh->setVertexBuffer("UV", 2, 4, uvBuffer);
  _secondTask.mesh->setPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
  
  _secondTextureMap->addTexture(_firstTask.frame->getColorTexture(0));
  _secondTask.textures = _secondTextureMap;
  
  fx::vec2 size = fx::vec2(_secondTask.frame->size());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _secondUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _secondUniform.view = fx::mat4();
  _secondUniform.model = fx::mat4::Scale(fx::vec3(0.8f, 0.8f, 0.8f));
  (*_secondUniformMap)["MVP"] = _secondUniform;
  _secondTask.uniforms.push_back(_secondUniformMap);
  
  _secondTask.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}
