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
  _renderScheme.push_back("MainPass");
}

void XMLScene::update(float td) {
  _model->setOrientation(_model->orientation() * fx::quat::RotZ(td));
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}

void XMLScene::render() {
  //_scene.render();
}
