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
#include "Graphics.h"
#include "GraphicResources.h"

DEFINE_CAMERA_BUILDER(Camera)


using namespace fx;
using namespace std;

Camera::Camera(Scene *scene): _scene(scene) {
  _frame = Graphics::getInstance().getMainWindowBuffer();
  _lightRig = nullptr;
  
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
  if (node.hasAttribute("pass"))
    _scene->renderPasses()[node.attribute("pass")].setCamera(this);
  if (node.hasAttribute("frame"))
    setFrame(node.attribute("frame"));
  if (node.hasAttribute("shader"))
    setShader(node.attribute("shader"));
  
  for (auto subnode : node) {
    if (subnode->element() == "Projection")
      success &= setProjection(*subnode);
    else if (subnode->element() == "View")
      success &= setView(*subnode);
    else if (subnode->element() == "Clear")
      success &= setClearState(*subnode);
  }
  return success;
}

bool Camera::preDraw() {
  return false;
}

void Camera::addDepthBuffer() {
  _frame->addDepthBuffer();
}

bool Camera::setView(const XMLTree::Node &node) {
  if (node.hasAttributes("eye", "center", "up")) {
    vec3 eye(node.attribute("eye"));
    vec3 center(node.attribute("center"));
    vec3 up(node.attribute("up"));
    lookAt(eye, center, up);
    return true;
  }
  return false;
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

bool Camera::setClearState(const XMLTree::Node &node) {
  if (node.hasAttribute("color"))
    setClearColor(node.attribute("color"));
  if (node.hasAttribute("depth"))
    setClearDepth(node.attributeAsFloat("depth"));
  return true;
}

void Camera::setLightRig(const string &name) {
  setLightRig(_scene->getLightRig(name));
}

void Camera::setFrame(const string &name) {
  FramePtr frame = _scene->getFrame(name);
  setFrame(frame);
}

void Camera::setShader(const string &name) {
  ShaderPtr shader = _scene->getShader(name);
  setShader(shader);
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

Camera* Camera::build(const string &type, Scene *scene) {
  if (builders().count(type))
    return builders().at(type)->build(scene);
  return nullptr;
}

CameraBuilderMap& Camera::builders() {
  static CameraBuilderMap builderMap;
  return builderMap;
}
