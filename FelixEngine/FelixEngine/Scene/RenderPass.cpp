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
#include "LightRig.h"


using namespace fx;
using namespace std;

RenderItem::RenderItem(Camera *camera, Model *model): camera(camera), model(model) {
  task.uniforms = &uniforms;
  setCamera(camera);
  setModel(model);
}

RenderItem& RenderItem::operator=(const RenderItem &other) {
  model = other.model;
  camera = other.camera;
  uniforms = other.uniforms;
  task = other.task;
  task.uniforms = &uniforms;
  return *this;
}

bool RenderItem::active() const {
  return !model->hidden() && task.mesh != nullptr && task.shader != nullptr && task.frame != nullptr;
}

void RenderItem::setClearOperations(Camera *camera) {
  if (camera != nullptr) {
    // Set Clear Color
    if (camera->isClearingColor())
      task.setClearColor(camera->clearColor());
    else
      task.setDefaultColorActions();
    
    // Set Clear Depth
    if (camera->isClearingDepth())
      task.setClearDepthStencil(camera->clearDepth());
    else
      task.setDefaultDepthStencilActions();
  }
}

void RenderItem::setDefaultOperations() {
  task.setDefaultColorActions();
  task.setDefaultDepthStencilActions();
}

void RenderItem::setCamera(Camera *newCamera) {
  if (camera != nullptr) {
    uniforms["camera"] = camera->data();
    task.frame = camera->frame();
    
    LightRig *lightRig = camera->lightRig();
    if (lightRig != nullptr && lightRig->size() > 0)
      uniforms["lights"] = lightRig->lights();
  }
}

void RenderItem::setModel(Model *newModel) {
  if (model != nullptr) {
    model = newModel;
    uniforms["model"] = model->data();
    task.mesh = model->mesh();
    
    Material *material = model->material();
    if (material != nullptr) {
      uniforms["material"] = material->data();
      task.textures = &material->textures();
      task.shader = material->shader();
      task.depthState = material->depthState();
    }
  }
}

void RenderItem::update() {
  if (model != nullptr) {
    uniforms["model"] = model->data();
    Material *material = model->material();
    if (material != nullptr)
      uniforms["material"] = material->data();
  }
  if (camera != nullptr) {
    uniforms["camera"] = camera->data();
    LightRig *lightRig = camera->lightRig();
    if (lightRig != nullptr && lightRig->size() > 0)
      uniforms["lights"] = lightRig->lights();
  }
  uniforms.update();
}


RenderPass::RenderPass(const std::string name): _name(name), _camera(nullptr) {
  
}

void RenderPass::setCamera(Camera *camera) {
  _camera = camera;
  for (auto item : _items)
    item.setCamera(_camera);
}

void RenderPass::addModel(Model *model) {
  _items.push_back(RenderItem(_camera, model));
}

void RenderPass::removeModel(Model *model) {
  // TODO
}

void RenderPass::update() {
  for (auto itr = _items.begin(); itr != _items.end(); ++itr)
    itr->update();
}

void RenderPass::render() {
  Graphics &graphics = Graphics::getInstance();
  bool firstItem = !_camera->preDraw();
  
  for (auto item : _items) {
    if (item.active()) {
      if (firstItem) {
        item.setClearOperations(_camera);
        firstItem = false;
      }
      else
        item.setDefaultOperations();
      graphics.addTask(item.task);
    }
  }
}



RenderPasses::RenderPasses() {
  
}

RenderPasses::~RenderPasses() {
  
}

void RenderPasses::update() {
  for (auto itr = _renderPasses.begin(); itr != _renderPasses.end(); ++itr)
    itr->update();
}

void RenderPasses::render() {
  for (auto itr = _renderPasses.begin(); itr != _renderPasses.end(); ++itr)
    itr->render();
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
