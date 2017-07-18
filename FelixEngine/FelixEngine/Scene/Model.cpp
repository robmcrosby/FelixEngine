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


Model::Model(): _data(1), _material(nullptr), _mesh(nullptr), _hidden(false) {

}

Model::~Model() {

}

void Model::update() {
  _data[0].model = mat4::Trans3d(_position) * _orientation.toMat4() * mat4::Scale(vec3(_scale, _scale, _scale));
  _data[0].rotation = _orientation;
}

bool Model::loadXML(const XMLTree::Node &node) {
  cout << node << endl;
  return true;
}

void Model::setShader(ShaderProgram *shader) {
  if (_material == nullptr)
    _material = new Material();
  _material->setShader(shader);
}

void Model::enableDepthTesting() {
  if (_material == nullptr)
    _material = new Material();
  _material->enableDepthTesting();
}
