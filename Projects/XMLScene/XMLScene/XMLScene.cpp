//
//  XMLScene.cpp
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "XMLScene.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>


XMLScene::XMLScene() {
  _transform = fx::Transform::make();
}

XMLScene::~XMLScene() {
  
}

void XMLScene::initalize() {
  _scene.loadXMLFile("Scene.xml");
  fx::ModelPtr model = _scene.get<fx::Model>("Model");
  model->transform()->setParrent(_transform);
  _renderScheme.push_back("MainPass");
}

void XMLScene::update(float td) {
  _transform->setRotation(_transform->localRotation() * fx::quat::RotZ(td));
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
