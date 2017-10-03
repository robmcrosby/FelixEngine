//
//  ARCamera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 8/5/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ARCamera.h"
#include "TrackerSystem.h"


using namespace fx;
using namespace std;


ARCameraBuilder ARCamera::arCameraBuilder = ARCameraBuilder();

ARCamera::ARCamera() {
  _uniformMap = make_shared<UniformMap>();
  TrackerSystem &tracker = TrackerSystem::getInstance();
  _imageY = tracker.getCameraImageY();
  _imageCbCr = tracker.getCameraImageCbCr();
  setupPreDraw();
}

ARCamera::~ARCamera() {
  
}

void ARCamera::setupPreDraw() {
  float positionBuffer[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, 0.0f, 1.0f
  };
  
  float uvBuffer[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  
  Graphics &graphics = Graphics::getInstance();
  _task.frame = graphics.getMainWindowBuffer();
  
  _task.shader = graphics.createShaderProgram();
  _task.shader->loadShaderFunctions("texture_vertex", "texture_fragment");
  
  _task.mesh = graphics.createVertexMesh();
  _task.mesh->addVertexBuffer("Position", 4, 4, positionBuffer);
  _task.mesh->addVertexBuffer("UV", 2, 4, uvBuffer);
  _task.mesh->setPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
  
  _task.textures = make_shared<TextureMap>();
  _task.textures->addTexture(_imageY);
  _task.textures->addTexture(_imageCbCr);
  _task.setClearDepthStencil();
  
  (*_uniformMap)["Transform"] = _imageTransform;
  _task.uniforms.push_back(_uniformMap);
  
  _task.colorActions[0].loadAction = LOAD_NONE;
}

bool ARCamera::cameraImagesReady() {
  return _imageY && _imageY->loaded() && _imageCbCr && _imageCbCr->loaded();
}

void ARCamera::update(float td) {
  Camera::update(td);
  
  TrackerSystem *tracker = &TrackerSystem::getInstance();
  if (tracker != nullptr) {
    setView(tracker->getCameraView());
    setProjection(tracker->getCameraProjection());
    _imageTransform = tracker->getImageTransform();
    (*_uniformMap)["Transform"] = _imageTransform;
  }

}

void ARCamera::applyToTask(GraphicTask &task) {
  if (cameraImagesReady()) {
    _task.frame = _frame;
    if (isClearingDepth())
      _task.setClearDepthStencil(clearDepth());
    Graphics::getInstance().addTask(_task);
  }
  else
    Camera::setupTemplateTask(task);
}


//
//DEFINE_CAMERA_BUILDER(ARCamera)
//
//
//ARCamera::ARCamera(Scene *scene): Camera(scene), _imageY(0), _imageCbCr(0) {
//  TrackerSystem *tracker = &TrackerSystem::getInstance();
//  if (tracker != nullptr) {
//    _imageY = tracker->getCameraImageY();
//    _imageCbCr = tracker->getCameraImageCbCr();
//    setupPreDraw();
//  }
//}
//
//ARCamera::~ARCamera() {
//
//}
//
//void ARCamera::setupPreDraw() {
//  float positionBuffer[] = {
//    -1.0f, -1.0f, 0.0f, 1.0f,
//    -1.0f,  1.0f, 0.0f, 1.0f,
//     1.0f, -1.0f, 0.0f, 1.0f,
//     1.0f,  1.0f, 0.0f, 1.0f
//  };
//
//  float uvBuffer[] = {
//    0.0f, 1.0f,
//    0.0f, 0.0f,
//    1.0f, 1.0f,
//    1.0f, 0.0f
//  };
//
//  Graphics &graphics = Graphics::getInstance();
//  _task.frame = graphics.getMainWindowBuffer();
//
//  _task.shader = graphics.createShaderProgram();
//  _task.shader->loadShaderFunctions("texture_vertex", "texture_fragment");
//
//  _task.mesh = graphics.createVertexMesh();
//  _task.mesh->addVertexBuffer("Position", 4, 4, positionBuffer);
//  _task.mesh->addVertexBuffer("UV", 2, 4, uvBuffer);
//  _task.mesh->setPrimativeType(fx::VERTEX_TRIANGLE_STRIP);
//
//  _task.textures = make_shared<TextureMap>();
//  _task.textures->addTexture(_imageY);
//  _task.textures->addTexture(_imageCbCr);
//  _task.setClearDepthStencil();
//
//  _task.uniforms = make_shared<UniformMap>();
//  (*_task.uniforms)["Transform"] = _imageTransform;
//  _task.uniforms->update();
//
//  _task.colorActions[0].loadAction = LOAD_NONE;
//}
//
//bool ARCamera::cameraImagesReady() {
//  return _imageY && _imageY->loaded() && _imageCbCr && _imageCbCr->loaded();
//}
//
//void ARCamera::update() {
//  TrackerSystem *tracker = &TrackerSystem::getInstance();
//  if (tracker != nullptr) {
//    setView(tracker->getCameraView());
//    setProjection(tracker->getCameraProjection());
//    _imageTransform = tracker->getImageTransform();
//    _uniformMap["Transform"] = _imageTransform;
//    _uniformMap.update();
//  }
//}
//
//bool ARCamera::preDraw() {
//  if (cameraImagesReady()) {
//    _task.frame = _frame;
//    if (isClearingDepth())
//      _task.setClearDepthStencil(clearDepth());
//    Graphics::getInstance().addTask(_task);
//    return true;
//  }
//  return false;
//}

