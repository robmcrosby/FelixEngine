//
//  CubeMap.cpp
//  CubeMap
//
//  Created by Robert Crosby on 11/25/20.
//

#include "CubeMap.h"
#include <FelixEngine/MeshBuilder.h>


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
  
  fx::RenderItem skybox;
  skybox.loadShaderFunctions("skybox_vertex", "basic_fragment");
  skybox.mesh = fx::MeshBuilder::createCube(100.0f);
  
  fx::RenderItem object;
  object.loadShaderFunctions("basic_vertex", "basic_fragment");
  object.loadMeshFile("bunnyMesh.mesh");
  object.cullMode = fx::CULL_BACK;
  object.enableDepthTesting();
  
  _renderPass = _graphics->createRenderPass();
  _renderPass->setFrameToWindow(0);
  _renderPass->addDepthBuffer();
  //_renderPass->setClearColor(fx::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  _renderPass->setClearDepthStencil();
  _renderPass->setCubeMapFiles("cubeMap", cubeMapFiles, sampler);
  //_renderPass->setCubeMapFile("cubeMap", "colorGrid.png", sampler);
  //_renderPass->setCubeMapFile("cubeMap", "environment.hdr", sampler);
  //_renderPass->setCubeMapFile("cubeMap", "environment2.jpeg", sampler);
  _renderPass->addRenderItem(skybox);
  _renderPass->addRenderItem(object);
  
  fx::vec2 size = fx::vec2(_renderPass->getFrameSize());
  float width = 0.2f;
  float height = 0.2f * size.h/size.w;
  _mvpUniform.projection = fx::mat4::Frustum(-width, width, -height, height, 1.0, 1000.0);
  _mvpUniform.rotation = fx::quat::RotX(M_PI/2.0f);
  _mvpUniform.model = _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.6f, 0.6f, 0.6f));
  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
}

void CubeMap::update(float td) {
  fx::quat rotation = motionSystem().getOrientation().inverse() * fx::quat::RotX(M_PI/2.0f);
  _mvpUniform.view = fx::mat4::Trans3d(fx::vec3(0.0f, 0.0f, -4.0)) * rotation.toMat4();
  _mvpUniform.camera = fx::vec4(rotation.inverse() * fx::vec3(0.0f, 0.0f, -4.0), 1.0);
  
  _mvpUniform.rotation = fx::quat::RotX(M_PI/2.0f) * _modelRotation;
  _mvpUniform.model =  _mvpUniform.rotation.toMat4() * fx::mat4::Scale(fx::vec3(0.5f, 0.5f, 0.5f)) * fx::mat4::Trans3d(fx::vec3(0.0, 0.0, 1.0));

  _renderPass->getUniformMap()["MVP"] = _mvpUniform;
  _renderPass->getUniformMap().update();
  
  _renderPass->render();
}

void CubeMap::handle(const fx::Event &event) {
  if (event.catagory == fx::EVENT_INPUT) {
    if (event.type == fx::INPUT_TOUCH_MOVE) {
      handleTouchMove(event);
    }
  }
}

void CubeMap::handleTouchMove(const fx::Event &event) {
  if (event.message.size() > 0) {
    const fx::Touch &touch = (const fx::Touch&)event.message[0];
    fx::vec2 diff = touch.pevious - touch.location;
    fx::vec3 up = _mvpUniform.view * fx::vec3(0.0, 1.0, 0.0);
    float dot = up.dot(fx::vec3(diff.y, diff.x, 0.0));
    _modelRotation *= fx::quat::RotZ(dot * 2.0);
  }
}
