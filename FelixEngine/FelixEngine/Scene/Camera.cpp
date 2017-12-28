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

CameraBuilder Camera::cameraBuilder = CameraBuilder();

Camera::Camera() {
  _scene = nullptr;
  _transform = Transform::make();
  _uniforms = UniformMap::make();;
  (*_uniforms)["camera"] = properties();
  
  _projectionType = PROJ_ORTHO;
  _near = 1.0;
  _far = 100.0;
  _scale = 1.0;
  _angle = Pi/4.0;
  
  _isClearingColor = false;
  _isClearingDepth = false;
}

Camera::~Camera() {
  
}

bool Camera::loadXML(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("pass"))
    addToRenderPass(node.attribute("pass"));
  if (node.hasAttribute("frame"))
    setFrame(node.attribute("frame"));
  if (node.hasAttribute("shader"))
    setShader(node.attribute("shader"));
  
  for (auto subnode : node)
    success &= loadXMLItem(*subnode);
  return success;
}

bool Camera::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Projection")
    return setProjection(node);
  if (node == "View")
    return setView(node);
  if (node == "Clear")
    return setClearState(node);
  if (node == "Frame")
    return setFrame(node);
  if (node == "Shader")
    return setShader(node);
  if (node == "Transform")
    return _transform->loadXML(node);
  return false;
}

STR_Camera Camera::properties() {
  STR_Camera properties;
  properties.projection = projection();
  properties.view = view();
  properties.position = vec4(globalLocation(), 1.0f);
  return properties;
}

void Camera::update(float dt) {
  (*_uniforms)["camera"] = properties();
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
  if (_frame)
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

void Camera::setView(const mat4 &view) {
  setRotation(view);
  setLocation(view.inverse() * vec3());
}

mat4 Camera::view() const {
  return (mat4::Trans3d(globalLocation()) * globalRotation().toMat4()).inverse();
}

bool Camera::setProjection(const XMLTree::Node &node) {
  if (node.hasAttributes("type", "far", "near")) {
    std::string type = node.attribute("type");
    float far = node.attributeAsFloat("far");
    float near = node.attributeAsFloat("near");
    if (type == "ortho") {
      float scale = node.attributeAsFloat("scale");
      setOrthographic(scale, near, far);
    }
    else if (type == "frustum") {
      float angle = node.attributeAsFloat("angle");
      setFrustum(angle, near, far);
    }
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

bool Camera::setFrame(const XMLTree::Node &node) {
  _frame = Graphics::getInstance().getFrameBuffer(node.attribute("name"));
  return _frame->loadXML(node);
}

void Camera::setFrame(const string &name) {
  _frame = Graphics::getInstance().getFrameBuffer(name);
}

bool Camera::setShader(const XMLTree::Node &node) {
  _shader = Graphics::getInstance().getShaderProgram(node.attribute("name"));
  return _shader->loadXML(node);
}

void Camera::setShader(const string &name) {
  _shader = Graphics::getInstance().getShaderProgram(name);
}

void Camera::setProjection(const mat4 &projection) {
  _projectionType = PROJ_MATRIX;
  _projection = projection;
}

void Camera::setOrthographic(float scale, float near, float far) {
  _projectionType = PROJ_ORTHO;
  _scale = scale;
  _near = near;
  _far = far;
}

void Camera::setFrustum(float angle, float near, float far) {
  _projectionType = PROJ_FRUSTUM;
  _angle = angle;
  _near = near;
  _far = far;
}

mat4 Camera::projection() const {
  switch (_projectionType) {
    case PROJ_ORTHO:
      return orthoProjection();
    case PROJ_FRUSTUM:
      return frustumProjection();
    case PROJ_MATRIX:
      return _projection;
  }
}

mat4 Camera::orthoProjection() const {
  fx::vec2 size = _frame ? fx::vec2(_frame->size()) : fx::vec2(1.0f, 1.0f);
  float w = _scale/2.0f;
  float h = (_scale * size.h/size.w)/2.0f;
  return mat4::Ortho(-w, w, -h, h, _near, _far);
}

mat4 Camera::frustumProjection() const {
  fx::vec2 size = _frame ? fx::vec2(_frame->size()) : fx::vec2(1.0f, 1.0f);
  float scale = _near * tanf(_angle/4.0f);
  float w = scale;
  float h = (scale * size.h/size.w);
  return mat4::Frustum(-w, w, -h, h, _near, _far);
}

void Camera::setClearColor(const vec4 &color) {
  _isClearingColor = true;
  _clearColor = color;
}

void Camera::setClearDepth(float depth) {
  _isClearingDepth = true;
  _clearDepth = depth;
}
