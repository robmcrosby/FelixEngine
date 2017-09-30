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
  printf("Create XMLScene\n");
}

XMLScene::~XMLScene() {
  
}

void XMLScene::initalize() {
  _scene.loadXMLFile("Scene.xml");
  _model = _scene.get<fx::Model>("Model");
}

void XMLScene::update() {
  _model->setOrientation(_model->orientation() * fx::quat::RotZ(0.02f));
  //_model->update();
  
  //_scene.update();
}

void XMLScene::render() {
  //_scene.render();
}
