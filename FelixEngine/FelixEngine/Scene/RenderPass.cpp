//
//  RenderPass.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/8/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "RenderPass.h"
#include "Graphics.h"

#include "Camera.h"
#include "Model.h"
#include "Material.h"


using namespace fx;
using namespace std;


RenderItem& RenderItem::operator=(const RenderItem &other) {
  model = other.model;
  uniforms = other.uniforms;
  task = other.task;
  task.uniforms = &uniforms;
  return *this;
}


RenderPass::RenderPass(const std::string name): _name(name), _camera(nullptr) {
  
}

void RenderPass::setCamera(Camera *camera) {
  _camera = camera;
  for (auto item : _items) {
    item.uniforms["Camera"] = _camera->data();
    item.task.frame = _camera->frame();
  }
}

void RenderPass::addModel(Model *model) {
  RenderItem item;
  item.model = model;
  item.uniforms["Model"] = model->data();
  item.task.mesh = model->mesh();
  
  Material *material = model->material();
  if (material != nullptr) {
    item.uniforms["Material"] = material->data();
    item.task.textures = &material->textures();
    item.task.shader = material->shader();
  }
  
  if (_camera != nullptr) {
    item.uniforms["Camera"] = _camera->data();
    item.task.frame = _camera->frame();
  }
  
  _items.push_back(item);
}

void RenderPass::removeModel(Model *model) {
  // TODO
}

void RenderPass::update() {
  for (auto item : _items)
    item.uniforms.update();
}

void RenderPass::render() {
  Graphics &graphics = Graphics::getInstance();
  for (auto item : _items) {
    graphics.addTask(item.task);
  }
}



RenderPasses::RenderPasses() {
  
}

RenderPasses::~RenderPasses() {
  
}

void RenderPasses::update() {
  for (auto pass : _renderPasses)
    pass.update();
}

void RenderPasses::render() {
  for (auto pass : _renderPasses)
    pass.render();
}

RenderPass& RenderPasses::operator[](const std::string &name) {
  // Attempt to find the RenderPass
  for (int i = 0; i < _renderPasses.size(); ++i) {
    if (name == _renderPasses.at(i).name())
      return _renderPasses.at(i);
  }
  
  // Create a new RenderPass
  _renderPasses.push_back(name);
  return _renderPasses.back();
}
