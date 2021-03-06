//
//  HelloTexture.cpp
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "HelloTexture.h"

HelloTexture::HelloTexture() {
  
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
  
  fx::RenderItem renderItem;
  
  // Load the Shader
  renderItem.loadShaderFunctions("basic_vertex", "basic_fragment");
  
  // Load the Texture
  renderItem.addTextureFile("test.png");
  
  // Load the Mesh
  renderItem.setMeshVertexBuffer("position", 3, 4, vertexBuffer);
  renderItem.setMeshVertexBuffer("uvMap", 2, 4, uvBuffer);
  renderItem.setMeshPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
  renderItem.loadMesh();
  
  // Setup the Render Pass
  _renderPass = _graphics->createRenderPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->addRenderItem(renderItem);
  
  // Setup the MVP Uniform
  fx::vec2 size = fx::vec2(_renderPass->getFrameSize());
  float width = 2.0f;
  float height = 2.0f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -100.0f, 100.0f);
  _mvpUniform.view = fx::mat4();
  _mvpUniform.model = fx::mat4::Scale(fx::vec3(0.8f, 0.8f, 0.8f));
  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
}

void HelloTexture::update(float td) {
  _renderPass->render();
}
