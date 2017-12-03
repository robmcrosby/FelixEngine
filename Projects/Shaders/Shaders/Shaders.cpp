//
//  Shaders.cpp
//  Shaders
//
//  Created by Robert Crosby on 12/2/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Shaders.h"
#include <FelixEngine/Camera.h>


Shaders::Shaders() {
  _transform = fx::Transform::make();
}

Shaders::~Shaders() {
  
}

void Shaders::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  fx::ModelPtr model = _scene.get<fx::Model>("ModelA");
  _rotation = model->transform()->localRotation();
  
  _models.push_back(model);
  _models.push_back(_scene.get<fx::Model>("ModelB"));
  _models.push_back(_scene.get<fx::Model>("ModelC"));
  _models.push_back(_scene.get<fx::Model>("ModelD"));
  _models.push_back(_scene.get<fx::Model>("ModelE"));
  _models.push_back(_scene.get<fx::Model>("ModelF"));
  _models.push_back(_scene.get<fx::Model>("ModelG"));
  _models.push_back(_scene.get<fx::Model>("ModelH"));
  _models.push_back(_scene.get<fx::Model>("ModelI"));
  
//  model->transform()->setParrent(_transform);
  _renderScheme.push_back("MainPass");
}

void Shaders::update(float td) {
  //_transform->setRotation(_transform->localRotation() * fx::quat::RotY(td));
  
  _rotation = _rotation * fx::quat::RotZ(-td);
  for (auto &model : _models)
    model->transform()->setRotation(_rotation);
  
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
