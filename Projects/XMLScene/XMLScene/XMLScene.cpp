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
  _scene.loadXMLFile("SceneB.xml");
  //fx::ModelPtr model = _scene.get<fx::Model>("Model");
  fx::ModelPtr model = _scene.get<fx::Model>("BakeObject");
  model->transform()->setParrent(_transform);
}

void XMLScene::update(float td) {
  _transform->setRotation(_transform->localRotation() * fx::quat::RotY(td));
  _scene.update(td);
}
