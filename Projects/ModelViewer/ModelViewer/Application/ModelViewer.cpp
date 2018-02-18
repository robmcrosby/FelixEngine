//
//  ModelViewer.cpp
//  ModelViewer
//
//  Created by Robert Crosby on 12/21/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ModelViewer.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>


ModelViewer::ModelViewer() {
  _transform = fx::Transform::make();
}

ModelViewer::~ModelViewer() {
  
}

void ModelViewer::initalize() {
  _scene.loadXMLFile("Scene.xml");
  fx::ModelPtr model = _scene.get<fx::Model>("Model");
  model->transform()->setParrent(_transform);
  _renderScheme.push_back("MainPass");
}

void ModelViewer::update(float td) {
  _transform->setRotation(_transform->localRotation() * fx::quat::RotY(td));
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
