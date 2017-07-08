//
//  Model.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Model.h"
#include "Material.h"


using namespace fx;
using namespace std;


Model::Model(): _data(1), _material(nullptr), _mesh(nullptr) {

}

Model::~Model() {

}

void Model::update() {
  
}

void Model::setShader(ShaderProgram *shader) {
  if (_material == nullptr)
    _material = new Material();
  _material->setShader(shader);
}
