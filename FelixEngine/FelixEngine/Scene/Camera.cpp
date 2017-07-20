//
//  Camera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Camera.h"
#include "Scene.h"
#include "LightRig.h"
#include "GraphicResources.h"


using namespace fx;
using namespace std;

Camera::Camera(Scene *scene): _scene(scene) {
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
  bool success = true;
  if (node.hasAttribute("lights"))
    setLightRig(node.attribute("lights"));
  for (auto subnode : node) {
    if (subnode->element() == "Projection")
      success &= setProjection(*subnode);
  }
  return success;
  
  //cout << node << endl;
  //return true;
}

bool Camera::setProjection(const XMLTree::Node &node) {
  if (node.hasAttributes("type", "far", "near", "scale")) {
    std::string type = node.attribute("type");
    float far = node.attributeAsFloat("far");
    float near = node.attributeAsFloat("near");
    float scale = node.attributeAsFloat("scale");
    if (type == "ortho")
      setOrthographic(scale, near, far);
    else if (type == "frustum")
      setFrustum(scale, near, far);
  }
  return true;
}

void Camera::setLightRig(const string &name) {
  setLightRig(_scene->getLightRig(name));
}

void Camera::setOrthographic(float scale, float near, float far) {
  fx::vec2 size = _frame != nullptr ? fx::vec2(_frame->size()) : fx::vec2(1.0f, 1.0f);
  float width = scale/2.0f;
  float height = (scale * size.h/size.w)/2.0f;
  setOrthographic(-width, width, -height, height, near, far);
}

void Camera::setFrustum(float scale, float near, float far) {
  fx::vec2 size = _frame != nullptr ? fx::vec2(_frame->size()) : fx::vec2(1.0f, 1.0f);
  float width = scale/2.0f;
  float height = (scale * size.h/size.w)/2.0f;
  setFrustum(-width, width, -height, height, near, far);
}

void Camera::setClearColor(const vec4 &color) {
  _isClearingColor = true;
  _clearColor = color;
}

void Camera::setClearDepth(float depth) {
  _isClearingDepth = true;
  _clearDepth = depth;
}
