//
//  Camera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//


#include "Camera.h"
#include "GraphicResources.h"


using namespace fx;
using namespace std;

CameraBuilder Camera::cameraBuilder = CameraBuilder();

Camera::Camera() {
  _scene = nullptr;
  _transform = Transform::make();
  
  _projectionType = PROJECTION_ORTHO;
  _near = 1.0;
  _far = 100.0;
  _scale = 1.0;
  _angle = Pi/4.0;
}

Camera::~Camera() {
  
}

bool Camera::loadXML(const XMLTree::Node &node) {
  bool success = true;
  // Set to a Render Pass
  if (node.hasAttribute("pass"))
    setToRenderPass(node.attribute("pass"));
  
  // Process the subNodes
  for (auto subnode : node)
    success &= loadXMLItem(*subnode);
  return success;
}

bool Camera::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Projection")
    return setProjection(node);
  if (node == "View")
    return setView(node);
  if (node == "RenderPass")
    return setToRenderPass(node);
  if (node == "Transform")
    return _transform->loadXML(node);
  return false;
}

STR_Camera Camera::properties(vec2 frameSize) {
  STR_Camera properties;
  properties.projection = projection(frameSize);
  properties.view = view();
  properties.position = vec4(globalLocation(), 1.0f);
  return properties;
}

void Camera::update(float dt) {
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass) {
    vec2 frameSize = (*pass)->getFrameSize();
    (*pass)->getUniformMap()["camera"] = properties(frameSize);
  }
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
  setLocation(view * vec3());
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

void Camera::setProjection(const mat4 &projection) {
  _projectionType = PROJECTION_MATRIX;
  _projection = projection;
}

void Camera::setOrthographic(float scale, float near, float far) {
  _projectionType = PROJECTION_ORTHO;
  _scale = scale;
  _near = near;
  _far = far;
}

void Camera::setFrustum(float angle, float near, float far) {
  _projectionType = PROJECTION_FRUSTUM;
  _angle = angle;
  _near = near;
  _far = far;
}

mat4 Camera::projection(vec2 frameSize) const {
  switch (_projectionType) {
    case PROJECTION_ORTHO:
      return orthoProjection(frameSize);
    case PROJECTION_FRUSTUM:
      return frustumProjection(frameSize);
    case PROJECTION_MATRIX:
      return _projection;
  }
}

mat4 Camera::orthoProjection(vec2 frameSize) const {
  float w = _scale/2.0f;
  float h = (_scale * frameSize.height/frameSize.width)/2.0f;
  return mat4::Ortho(-w, w, -h, h, _near, _far);
}

mat4 Camera::frustumProjection(vec2 frameSize) const {
  float scale = _near * tanf(_angle/4.0f);
  float w = scale;
  float h = (scale * frameSize.height/frameSize.width);
  return mat4::Frustum(-w, w, -h, h, _near, _far);
}
