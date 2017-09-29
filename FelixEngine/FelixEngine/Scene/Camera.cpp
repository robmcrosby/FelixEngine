//
//  Camera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//


#include "Camera.h"
#include "GraphicTask.h"
#include "GraphicResources.h"
#include "RenderPass.h"


using namespace fx;
using namespace std;

Camera::Camera() {
  _scene = nullptr;
  _frame = Graphics::getInstance().getMainWindowBuffer();
  _uniforms = make_shared<UniformMap>();
  (*_uniforms)["camera"] = _properties;
  
  _isClearingColor = false;
  _isClearingDepth = false;
}

Camera::~Camera() {
  
}

bool Camera::loadXML(const XMLTree::Node &node) {
  return true;
}

void Camera::update(float dt) {
  (*_uniforms)["camera"] = _properties;
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
    (*pass)->setCamera(this);
}

void Camera::setupTemplateTask(GraphicTask &task) {
  task.frame = _frame;
  task.uniforms.push_back(_uniforms);
}

void Camera::applyToTask(GraphicTask &task) {
  // Set Clear Color
  if (isClearingColor())
    task.setClearColor(clearColor());
  else
    task.setDefaultColorActions();
  
  // Set Clear Depth
  if (isClearingDepth())
    task.setClearDepthStencil(clearDepth());
  else
    task.setDefaultDepthStencilActions();
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

void Camera::setFrame(const string &name) {
  _frame = Graphics::getInstance().getFrameBuffer(name);
}

void Camera::setShader(const string &name) {
  _shader = Graphics::getInstance().getShaderProgram(name);
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
