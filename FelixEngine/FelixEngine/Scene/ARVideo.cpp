//
//  ARVideo.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/28/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "ARVideo.h"
#include "TrackerSystem.h"
#include "MeshBuilder.h"


using namespace fx;
using namespace std;


ARVideoBuilder ARVideo::arVideoBuilder = ARVideoBuilder();

ARVideo::ARVideo() {
  
}

ARVideo::~ARVideo() {
  
}

bool ARVideo::load(const XMLTree::Node &node) {
  TrackerSystem *tracker = &TrackerSystem::getInstance();
  if (!tracker) {
    cerr << "An Initialized Tracking System Must be present when using ARVideo" << endl;
    assert(false);
  }
  _renderItem.setTexture("capturedImageTextureY", tracker->getCameraImageY());
  _renderItem.setTexture("capturedImageTextureCbCr", tracker->getCameraImageCbCr());
  _renderItem.mesh = MeshBuilder::createPlane();
  _renderItem.loadShaderFunctions("video_vertex", "video_fragment_Y_CbCr");
  _renderItem.getUniformMap()["transform"] = tracker->getImageTransform();
  
  return Model::load(node);
}

void ARVideo::update(float td) {
  TrackerSystem *tracker = &TrackerSystem::getInstance();
  if (tracker != nullptr) {
    _renderItem.getUniformMap()["transform"] = tracker->getImageTransform();
    _renderItem.getUniformMap().update();
  }
  Model::update(td);
}
