//
//  ARKitTracker.mm
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ARKitTracker.h"
#include <iostream>
#include <ARKit/ARKit.h>


@interface ARDelegate : NSObject <ARSessionDelegate>
@property (nonatomic, assign) fx::ARKitTracker *tracker;
@end

@implementation ARDelegate

- (void)session:(ARSession *)session didFailWithError:(NSError *)error {
  self.tracker->arSessionFailed();
}

- (void)sessionWasInterrupted:(ARSession *)session {
  self.tracker->arSessionInterupted();
}

- (void)sessionInterruptionEnded:(ARSession *)session {
  self.tracker->arSessionInteruptEnd();
}

- (void)session:(ARSession *)session cameraDidChangeTrackingState:(ARCamera *)camera {
  if (camera.trackingState == ARTrackingStateLimited)
    self.tracker->setTrackingStatus(fx::TRACKING_LIMITED);
  else if (camera.trackingState == ARTrackingStateNormal)
    self.tracker->setTrackingStatus(fx::TRACKING_NORMAL);
  else
    self.tracker->setTrackingStatus(fx::TRACKING_NOT_AVALIBLE);
}

@end


using namespace fx;
using namespace std;

ARKitTracker::ARKitTracker() {
  instance = this;
  
  _graphics = nullptr;
  _firstAnchor = nil;
  
  _arDelegate = [ARDelegate new];
  [_arDelegate setTracker:this];
  
  _arSession = [ARSession new];
  [_arSession setDelegate:_arDelegate];
}

ARKitTracker::~ARKitTracker() {
  
}

bool ARKitTracker::initalize(MetalGraphics *graphics) {
  _graphics = graphics;
  
  ARSessionConfiguration *configuration = [ARWorldTrackingSessionConfiguration new];
  [_arSession runWithConfiguration: configuration];
  
  return true;
}

mat4 ARKitTracker::getCameraView() {
  matrix_float4x4 view = matrix_invert(_arSession.currentFrame.camera.transform);
  return mat4::RotZ(-Pi/2.0f) * mat4((float*)&view.columns[0]);
}

mat4 ARKitTracker::getCameraProjection() {
  CGSize viewport = UIScreen.mainScreen.bounds.size;
  UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
  matrix_float4x4 proj = [_arSession.currentFrame.camera projectionMatrixWithViewportSize:viewport
                                                                              orientation:orientation
                                                                                    zNear:0.001
                                                                                     zFar:1000.0];
  return (float*)&proj.columns[0];
}

void ARKitTracker::arSessionFailed() {
  cout << "AR Session Failed With Error" << endl;
}

void ARKitTracker::arSessionInterupted() {
  cout << "AR Session Interupted" << endl;
}

void ARKitTracker::arSessionInteruptEnd() {
  cout << "AR Session Interuption End" << endl;
}

void ARKitTracker::setTrackingStatus(TRACKING_STATUS status) {
  if (_firstAnchor == nil && status == TRACKING_NORMAL) {
    ARFrame *frame = _arSession.currentFrame;
    if (frame != nil) {
      matrix_float4x4 transform = frame.camera.transform;
      _firstAnchor = [[ARAnchor alloc] initWithTransform:transform];
      [_arSession addAnchor:_firstAnchor];
    }
  }
  _trackingStatus = status;
}
