//
//  Camera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Camera.h"
#include "LightRig.h"


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

void Camera::setClearColor(const vec4 &color) {
  _isClearingColor = true;
  _clearColor = color;
}

void Camera::setClearDepth(float depth) {
  _isClearingDepth = true;
  _clearDepth = depth;
}
