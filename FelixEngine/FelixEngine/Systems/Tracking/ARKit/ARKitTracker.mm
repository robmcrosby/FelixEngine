//
//  ARKitTracker.mm
//  FelixEngine
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ARKitTracker.h"
#include "MetalGraphics.h"
#include "MetalTextureBuffer.h"
#include "Event.h"
#include <iostream>
#include <ARKit/ARKit.h>


@interface ARDelegate : NSObject <ARSessionDelegate>
@property (nonatomic, assign) fx::ARKitTracker *tracker;
@property (nonatomic) CVMetalTextureCacheRef textureCache;

@property (nonatomic) ARSession *arSession;
@property (nonatomic) BOOL sessionRunning;
@property (nonatomic) ARWorldTrackingConfiguration *configuration;
@end

@implementation ARDelegate

- (id)init {
  self = [super init];
  if (self != nil) {
    self.configuration = [ARWorldTrackingConfiguration new];
    self.arSession = [ARSession new];
    [self.arSession setDelegate:self];
  }
  return self;
}

- (void)startSession {
  [self.arSession runWithConfiguration:self.configuration];
  self.sessionRunning = YES;
}

- (void)setFeatureTracking:(u_int32_t)flags {
  // Set Plane Tracking
  self.configuration.planeDetection = flags & fx::FEATURE_TRACKING_PLANES_HORIZONTAL ? ARPlaneDetectionHorizontal : ARPlaneDetectionNone;
  if (@available(iOS 11.3, *))
    self.configuration.planeDetection |= flags & fx::FEATURE_TRACKING_PLANES_VERTICAL ? ARPlaneDetectionVertical : ARPlaneDetectionNone;
  
  // Update ARSession if Running
  if (self.sessionRunning)
    [_arSession runWithConfiguration:self.configuration];
}

- (fx::mat4)getCameraView {
  UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
  matrix_float4x4 view = matrix_invert([_arSession.currentFrame.camera viewMatrixForOrientation:orientation]);
  return (float*)&view.columns[0];
}

- (fx::mat4)getCameraProjection {
  CGSize viewport = UIScreen.mainScreen.bounds.size;
  UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
  matrix_float4x4 proj = [_arSession.currentFrame.camera projectionMatrixForOrientation:orientation
                                                                           viewportSize:viewport
                                                                                  zNear:0.001
                                                                                   zFar:1000.0];
  return (float*)&proj.columns[0];
}

- (fx::mat4)getImageTransform {
  fx::mat4 transform;
  ARFrame *frame = _arSession.currentFrame;
  if (frame != nil) {
    CGSize viewport = UIScreen.mainScreen.bounds.size;
    CGAffineTransform affine = [frame displayTransformForOrientation:UIInterfaceOrientationPortrait viewportSize:viewport];
    affine = CGAffineTransformInvert(affine);
    transform.x.x = affine.a;
    transform.x.y = affine.b;
    transform.y.x = affine.c;
    transform.y.y = affine.d;
    transform.w.x = affine.tx;
    transform.w.y = affine.ty;
  }
  return transform;
}

- (void)session:(ARSession *)session didFailWithError:(NSError *)error {
  NSLog(@"%@", error.localizedRecoverySuggestion);
  self.sessionRunning = NO;
  self.tracker->arSessionFailed();
}

- (void)sessionWasInterrupted:(ARSession *)session {
  self.sessionRunning = NO;
  self.tracker->arSessionInterupted();
}

- (void)sessionInterruptionEnded:(ARSession *)session {
  self.sessionRunning = NO;
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

- (void)session:(ARSession *)session didUpdateFrame:(ARFrame *)frame {
  self.tracker->arSessionUpdateFrame(frame);
}

- (void)session:(ARSession *)session didAddAnchors:(NSArray<ARAnchor *> *)anchors {
  for (ARAnchor *anchor : anchors) {
    if ([anchor isKindOfClass:[ARPlaneAnchor class]])
      self.tracker->planeAnchorAdded((ARPlaneAnchor*)anchor);
  }
}

- (void)session:(ARSession *)session didUpdateAnchors:(NSArray<ARAnchor *> *)anchors {
  for (ARAnchor *anchor : anchors) {
    if ([anchor isKindOfClass:[ARPlaneAnchor class]])
      self.tracker->planeAnchorUpdated((ARPlaneAnchor*)anchor);
  }
}

- (void)session:(ARSession *)session didRemoveAnchors:(NSArray<ARAnchor *> *)anchors {
  for (ARAnchor *anchor : anchors) {
    if ([anchor isKindOfClass:[ARPlaneAnchor class]])
      self.tracker->planeAnchorRemoved((ARPlaneAnchor*)anchor);
  }
}

//- (void)findPlaneForTouchPosX:(float)x posY:(float)y {
//  //self.tracker
//}

@end


using namespace fx;
using namespace std;

ARKitTracker::ARKitTracker() {
  instance = this;
  
  _graphics = nullptr;
  _arDelegate = [ARDelegate new];
  [_arDelegate setTracker:this];
  
  _uniformMap = UniformMap::make();
}

ARKitTracker::~ARKitTracker() {
  
}

bool ARKitTracker::initalize(MetalGraphics *graphics) {
  _graphics = graphics;
  
  id <MTLDevice> device = MTLCreateSystemDefaultDevice();
  CVMetalTextureCacheRef textureCache;
  CVMetalTextureCacheCreate(NULL, NULL, device, NULL, &textureCache);
  _arDelegate.textureCache = textureCache;
  
  _cameraImageY = dynamic_pointer_cast<MetalTextureBuffer>(graphics->createTextureBuffer());
  _cameraImageCbCr = dynamic_pointer_cast<MetalTextureBuffer>(graphics->createTextureBuffer());
  
  [_arDelegate startSession];
  return true;
}

mat4 ARKitTracker::getCameraView() {
  return [_arDelegate getCameraView];
}

mat4 ARKitTracker::getCameraProjection() {
  return [_arDelegate getCameraProjection];
}

mat4 ARKitTracker::getImageTransform() {
  return [_arDelegate getImageTransform];
}

void ARKitTracker::enableFeatureTracking(unsigned int featureFlags) {
  TrackerSystem::enableFeatureTracking(featureFlags);
  [_arDelegate setFeatureTracking:_freatureTrackingFlags];
}

void ARKitTracker::disableFeatureTracking(unsigned int featureFlags) {
  TrackerSystem::disableFeatureTracking(featureFlags);
  [_arDelegate setFeatureTracking:_freatureTrackingFlags];
}

const ARPoints& ARKitTracker::getPointCloud() const {
  return _pointCloud;
}

TextureBufferPtr ARKitTracker::getCameraImageY() {
  return _cameraImageY;
}

TextureBufferPtr ARKitTracker::getCameraImageCbCr() {
  return _cameraImageCbCr;
}

ARHitResults ARKitTracker::hitTest(const Touch &touch) {
  CGPoint touchPoint = CGPointMake(touch.location.x, touch.location.y);
  NSArray *hitTestResults = [_arDelegate.arSession.currentFrame hitTest:touchPoint types:ARHitTestResultTypeExistingPlane];
  ARHitResults hitResults;
  
  for (ARHitTestResult *result : hitTestResults) {
    ARPlaneAnchor *anchor = (ARPlaneAnchor*)result.anchor;
    matrix_float4x4 world = result.worldTransform;
    matrix_float4x4 local = result.localTransform;
    
    ARHitResult hitResult;
    hitResult.plane = planeForAnchor(anchor);
    hitResult.world = mat4((float*)&world.columns[0]);
    hitResult.local = mat4((float*)&local.columns[0]);
    hitResults.push_back(hitResult);
  }
  return hitResults;
}

const ARPlanes& ARKitTracker::trackedPlanes() const {
  return _trackedPlanes;
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

void ARKitTracker::arSessionUpdateFrame(ARFrame *frame) {
  CVPixelBufferRef pixelBuffer = frame.capturedImage;
  
  if (CVPixelBufferGetPlaneCount(pixelBuffer) >= 2) {
    CVMetalTextureRef texture = NULL;
    
    size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
    size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, _arDelegate.textureCache, pixelBuffer, NULL, MTLPixelFormatR8Unorm, width, height, 0, &texture);
    if(status == kCVReturnSuccess) {
      _cameraImageY->setMetalTexture(CVMetalTextureGetTexture(texture));
      CFRelease(texture);
    }
    
    width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
    height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
    status = CVMetalTextureCacheCreateTextureFromImage(NULL, _arDelegate.textureCache, pixelBuffer, NULL, MTLPixelFormatRG8Unorm, width, height, 1, &texture);
    if(status == kCVReturnSuccess) {
      _cameraImageCbCr->setMetalTexture(CVMetalTextureGetTexture(texture));
      CFRelease(texture);
    }
  }
  
  if (pointCloudEnabled()) {
    ARPointCloud *pointCloud = [frame rawFeaturePoints];
    long size = [pointCloud count];
    _pointCloud.resize(size);
    for (long i = 0; i < [pointCloud count]; ++i) {
      const vector_float3 &pt = pointCloud.points[i];
      _pointCloud[i].position = vec3(pt[0], pt[1], pt[2]);
      _pointCloud[i].identifier = pointCloud.identifiers[i];
    }
  }
}

void ARKitTracker::setTrackingStatus(TRACKING_STATUS status) {
  _trackingStatus = status;
}

void ARKitTracker::planeAnchorAdded(ARPlaneAnchor *anchor) {
  ARPlane plane = planeForAnchor(anchor);
  updateTrackedPlane(plane);
  if (_delegate != nullptr)
    _delegate->trackedPlaneAdded(plane);
}

void ARKitTracker::planeAnchorUpdated(ARPlaneAnchor *anchor) {
  ARPlane plane = planeForAnchor(anchor);
  updateTrackedPlane(plane);
  if (_delegate != nullptr)
    _delegate->trackedPlaneUpdated(plane);
}

void ARKitTracker::planeAnchorRemoved(ARPlaneAnchor *anchor) {
  ARPlane plane = planeForAnchor(anchor);
  updateTrackedPlane(plane);
  if (_delegate != nullptr)
    _delegate->trackedPlaneRemoved(plane);
}

ARPlane ARKitTracker::planeForAnchor(ARPlaneAnchor *anchor) {
  matrix_float4x4 transform = anchor.transform;
  vector_float3 center = anchor.center;
  
  ARPlane plane;
  plane.uuid = [[anchor.identifier UUIDString] UTF8String];
  plane.transform = mat4((float*)&transform.columns[0]);
  plane.center = vec3(center.x, center.y, center.z);
  plane.position = plane.transform.w.xyz();
  plane.rotation = quat(plane.transform);
  plane.extent = vec2(anchor.extent.x, anchor.extent.z);
  
  return plane;
}

void ARKitTracker::updateTrackedPlane(const ARPlane &plane) {
  for (auto &tracked : _trackedPlanes) {
    if (tracked.uuid == plane.uuid) {
      tracked = plane;
      return;
    }
  }
  _trackedPlanes.push_back(plane);
}
