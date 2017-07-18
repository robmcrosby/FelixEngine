//
//  Camera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Camera.h"
#include "LightRig.h"
#include "GraphicResources.h"


using namespace fx;
using namespace std;

Camera::Camera() {
  _lightRig = nullptr;
  _frame = nullptr;
  
  _isClearingColor = false;
  _isClearingDepth = false;
}

Camera::~Camera() {
  
}

void Camera::update() {
  
}

bool Camera::loadXML(const XMLTree::Node &node) {
  cout << node << endl;
  return true;
}

void Camera::setOrthographic(float scale, float near, float far) {
  fx::vec2 size = _frame != nullptr ? fx::vec2(_frame->size()) : fx::vec2(1.0f, 1.0f);
  float width = scale/2.0f;
  float height = (scale * size.h/size.w)/2.0f;
  setOrthographic(-width, width, -height, height, near, far);
}

void Camera::setClearColor(const vec4 &color) {
  _isClearingColor = true;
  _clearColor = color;
}

void Camera::setClearDepth(float depth) {
  _isClearingDepth = true;
  _clearDepth = depth;
}
