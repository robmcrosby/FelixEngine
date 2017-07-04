//
//  HelloTexture.cpp
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "HelloTexture.h"

HelloTexture::HelloTexture() {
  printf("Create HelloTexture\n");
}

HelloTexture::~HelloTexture() {
  
}

void HelloTexture::initalize() {
  float vertexBuffer[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f
  };
  
  float uvBuffer[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
  };
  
  _task.frame = _graphics->getMainWindowBuffer();
  
  _task.shader = _graphics->createShaderProgram();
  _task.shader->loadShaderFunctions("basic_vertex", "basic_fragment");
  
  _task.mesh = _graphics->createVertexMesh();
  _task.mesh->addVertexBuffer("Position", 4, 4, vertexBuffer);
  _task.mesh->addVertexBuffer("UV", 2, 4, uvBuffer);
  _task.mesh->setPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
  
  fx::ImageBufferData image;
  fx::FileSystem::loadImage(image, "bunnies.png");
  
  fx::vec2 size = fx::vec2(_task.frame->size());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _mvpUniform.view = fx::mat4();
  _mvpUniform.model = fx::mat4::Scale(fx::vec3(0.8f, 0.8f, 0.8f));
  
  _task.uniforms["MVP"] = _mvpUniform;
  
  _task.setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void HelloTexture::render() {
  _graphics->addTask(_task);
}
