//
//  Shaders.cpp
//  Shaders
//
//  Created by Robert Crosby on 12/2/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Shaders.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>


Shaders::Shaders() {
  _transform = fx::Transform::make();
}

Shaders::~Shaders() {
  
}

void Shaders::initalize() {
  _scene.loadXMLFile("Scene.xml");
  fx::ModelPtr model = _scene.get<fx::Model>("Model");
  model->transform()->setParrent(_transform);
  _renderScheme.push_back("MainPass");
}

void Shaders::update(float td) {
  _transform->setRotation(_transform->localRotation() * fx::quat::RotY(td));
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
