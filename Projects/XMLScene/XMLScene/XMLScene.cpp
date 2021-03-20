//
//  XMLScene.cpp
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "XMLScene.h"


using namespace std;

XMLScene::XMLScene() {
  _transform = fx::Transform::make();
  //_transform->setScale(2.0f);
}

XMLScene::~XMLScene() {
  
}

void XMLScene::initalize() {
  _scene.loadXMLFile("SceneB.xml");
  //_scene.loadXMLFile("Scene.xml");
  //_model = _scene.get<fx::Model>("Model");
  //_model = _scene.get<fx::Model>("BakeObject");
  //_model->transform()->setParrent(_transform);
  
  _camera = _scene.get<fx::Camera>("Camera");
}

void XMLScene::update(float td) {
  //_transform->setRotation(_transform->localRotation() * fx::quat::RotY(td));
  _scene.update(td);
}

void XMLScene::handle(const fx::Event &event) {
  if (event.catagory == fx::EVENT_INPUT) {
    if (event.type == fx::INPUT_TOUCH_MOVE) {
      handleTouchMove(event);
    }
  }
}

void XMLScene::handleTouchMove(const fx::Event &event) {
  if (event.message.size() > 0) {
    const fx::Touch &touch = (const fx::Touch&)event.message[0];
    fx::vec2 diff = touch.pevious - touch.location;
    fx::vec3 up = _camera->view() * fx::vec3(0.0, 0.0, 1.0);
    float dot = up.dot(fx::vec3(diff.x, diff.y, 0.0));
    _transform->setOrientation(_transform->rotation() * fx::quat::RotY(dot * 4.0));
  }
}
