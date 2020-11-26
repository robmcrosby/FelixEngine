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
  sampler.setMagFilter(fx::FILTER_LINEAR);
  sampler.setMinFilter(fx::FILTER_LINEAR);
  
  fx::RenderItem renderItem;
  renderItem.loadShaderFunctions("basic_vertex", "basic_fragment");
  renderItem.loadMeshFile("bunnyMesh.mesh");
  renderItem.setCubeMapFiles("cubeMap", cubeMapFiles, sampler);
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
  _mvpUniform.rotation = fx::quat::RotY(M_PI); //fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.6f, 0.6f, 0.6f));
  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
}

void CubeMap::update(float td) {
  // Rotate the Model
  //_mvpUniform.rotation *= fx::quat::RotZ(0.02f);
  //_mvpUniform.rotation = motionSystem().getOrientation();
  fx::mat4 rotation = motionSystem().getOrientation().inverse().toMat4();
  _mvpUniform.view = fx::mat4::Trans3d(fx::vec3(0.0f, 0.0f, -10.0)) * rotation;
  _mvpUniform.model =  _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.5f, 0.5f, 0.5f)) * fx::mat4::Trans3d(fx::vec3(0.0, 0.0, 1.0f));

  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
  _renderPass->getUniformMap().update();
  
  _renderPass->render();
}
