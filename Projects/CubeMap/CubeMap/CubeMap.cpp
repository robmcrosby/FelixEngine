//
//  CubeMap.cpp
//  CubeMap
//
//  Created by Robert Crosby on 11/25/20.
//

#include "CubeMap.h"


using namespace std;

CubeMap::CubeMap() {
  
}

CubeMap::~CubeMap() {
  
}

void CubeMap::initalize() {
  vector<string> cubeMapFiles;
  cubeMapFiles.push_back("px.png");
  cubeMapFiles.push_back("nx.png");
  cubeMapFiles.push_back("py.png");
  cubeMapFiles.push_back("ny.png");
  cubeMapFiles.push_back("pz.png");
  cubeMapFiles.push_back("nz.png");
  
  fx::SamplerState sampler;
  sampler.enableMipMapping();
  sampler.setMinFilter(fx::FILTER_LINEAR);
  sampler.setMagFilter(fx::FILTER_LINEAR);
  sampler.setMinFilter(fx::FILTER_LINEAR);
  
  fx::RenderItem renderItem;
  renderItem.loadShaderFunctions("basic_vertex", "basic_fragment");
  renderItem.loadMeshFile("bunnyMesh.mesh");
  renderItem.setCubeMapFiles("cubeMap", cubeMapFiles, sampler);
  //renderItem.setCubeMapFile("cubeMap", "colorGrid.png", sampler);
  //renderItem.setCubeMapFile("cubeMap", "environment.hdr", sampler);
  renderItem.cullMode = fx::CULL_BACK;
  renderItem.enableDepthTesting();
  
  _renderPass = _graphics->createRenderPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->addDepthBuffer();
  _renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->setClearDepthStencil();
  _renderPass->addRenderItem(renderItem);
  
  fx::vec2 size = fx::vec2(_renderPass->getFrameSize());
  float width = 0.2f;
  float height = 0.2f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Frustum(-width, width, -height, height, 1.0, 100.0);
  _mvpUniform.rotation = fx::quat::RotX(M_PI/2.0f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.6f, 0.6f, 0.6f));
  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
}

void CubeMap::update(float td) {
  fx::quat rotation = motionSystem().getOrientation().inverse() * fx::quat::RotX(M_PI/2.0f);
  _mvpUniform.view = fx::mat4::Trans3d(fx::vec3(0.0f, 0.0f, -4.0)) * rotation.toMat4();
  _mvpUniform.camera = fx::vec4(rotation.inverse() * fx::vec3(0.0f, 0.0f, -4.0), 1.0);
  _mvpUniform.model =  _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.5f, 0.5f, 0.5f)) * fx::mat4::Trans3d(fx::vec3(0.0, 0.0, 1.0f));

  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
  _renderPass->getUniformMap().update();
  
  _renderPass->render();
}
