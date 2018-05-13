//
//  HelloTexture.cpp
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "HelloTexture.h"

HelloTexture::HelloTexture() {
  _uniformMap = std::make_shared<fx::UniformMap>();
  _textureMap = std::make_shared<fx::TextureMap>();
}

HelloTexture::~HelloTexture() {
  
}

void HelloTexture::initalize() {
  float vertexBuffer[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f
  };
  
  float uvBuffer[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  
  _task.frame = _graphics->getFrameBuffer("MainWindow");
  _task.frame->setToWindow(0);
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->setVertexBuffer("position", 3, 4, vertexBuffer);
  _task.mesh->setVertexBuffer("uvMap", 2, 4, uvBuffer);
  _task.mesh->setPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
  _task.mesh->loadBuffers();
  
  fx::ImageBufferData image;
  fx::FileSystem::loadImage(image, "test.png");
  _textureMap->addTexture(image);
  _task.textures = _textureMap;
  
  fx::vec2 size = fx::vec2(_task.frame->size());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _mvpUniform.view = fx::mat4();
  _mvpUniform.model = fx::mat4::Scale(fx::vec3(0.8f, 0.8f, 0.8f));
  (*_uniformMap)["MVP"] = _mvpUniform;
  _task.uniforms.push_back(_uniformMap);
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void HelloTexture::update(float td) {
  _graphics->addTask(_task);
}
