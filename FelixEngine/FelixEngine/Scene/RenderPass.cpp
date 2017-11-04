//
//  RenderPass.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/8/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "RenderPass.h"


using namespace fx;
using namespace std;

RenderPass::RenderPass() {
  _camera = nullptr;
  _lightUniforms = UniformMap::make();
}

RenderPass::~RenderPass() {
  
}

void RenderPass::render() {
  Graphics &graphics = Graphics::getInstance();

  if (_models.size()) {
    auto model = _models.begin();
    GraphicTask templateTask;
    
    // Setup the template
    if (_camera)
      _camera->setupTemplateTask(templateTask);
    
    // Update and assign light uniforms
    updateLightUniforms();
    templateTask.uniforms.push_back(_lightUniforms);

    // Apply Camera clear operations on the first task
    if (_camera) {
      GraphicTask task = templateTask;
      (*model)->applyToTask(task);
      _camera->applyToTask(task);
      graphics.addTask(task);
      ++model;
    }

    // Create tasks for the rest of the models
    while (model != _models.end()) {
      GraphicTask task = templateTask;
      (*model)->applyToTask(task);
      graphics.addTask(task);
      ++model;
    }
  }
}

void RenderPass::updateLightUniforms() {
  vector<LightPeramaters> lightPeramaters;
  for (auto &light : _lights)
    lightPeramaters.push_back(light->peramaters());
  (*_lightUniforms)["lights"] = lightPeramaters;
}

void RenderPass::reset() {
  _camera = nullptr;
  _lights.clear();
  _models.clear();
}

RenderPassMap& RenderPass::getRenderPassMap() {
  static RenderPassMap mapInstance = RenderPassMap();
  return mapInstance;
}

RenderPassPtr RenderPass::getRenderPass(const std::string &name) {
  RenderPassMap &map = getRenderPassMap();
  RenderPassPtr renderPass = map[name];
  if (!renderPass) {
    renderPass = std::make_shared<RenderPass>();
    map[name] = renderPass;
  }
  return renderPass;
}

void RenderPass::renderPasses(const RenderScheme &scheme) {
  RenderScheme::const_iterator name = scheme.begin();
  while (name != scheme.end()) {
    RenderPassPtr pass = getRenderPass(*name);
    pass->render();
    ++name;
  }
}

void RenderPass::resetPasses() {
  RenderPassMap &passes = getRenderPassMap();
  RenderPassMap::iterator pass = passes.begin();
  while (pass != passes.end()) {
    pass->second->reset();
    ++pass;
  }
}
