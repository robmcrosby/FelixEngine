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
}

Shaders::~Shaders() {
  
}

void Shaders::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  _models.push_back(_scene.get<fx::Model>("ModelA"));
  _models.push_back(_scene.get<fx::Model>("ModelB"));
  _models.push_back(_scene.get<fx::Model>("ModelC"));
  _models.push_back(_scene.get<fx::Model>("ModelD"));
  _models.push_back(_scene.get<fx::Model>("ModelE"));
  _models.push_back(_scene.get<fx::Model>("ModelF"));
  _models.push_back(_scene.get<fx::Model>("ModelG"));
  _models.push_back(_scene.get<fx::Model>("ModelH"));
  _models.push_back(_scene.get<fx::Model>("ModelI"));
  
  _rotation = _models.front()->orientation(); //_models.front()->transform()->localRotation();
}

void Shaders::update(float td) {
  _rotation = _rotation * fx::quat::RotZ(-td);
  for (auto &model : _models)
    model->setOrientation(_rotation);
    //model->transform()->setRotation(_rotation);
  
  _scene.update(td);
}
