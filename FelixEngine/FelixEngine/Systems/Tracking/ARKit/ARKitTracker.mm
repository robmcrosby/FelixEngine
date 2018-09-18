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
#include <iostream>
#include <ARKit/ARKit.h>


@interface ARDelegate : NSObject <ARSessionDelegate>
@property (nonatomic, assign) fx::ARKitTracker *tracker;
@property (nonatomic) CVMetalTextureCacheRef textureCache;
@end

@implementation ARDelegate

- (void)session:(ARSession *)session didFailWithError:(NSError *)error {
  NSLog(@"%@", error.localizedRecoverySuggestion);
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
  
  _pointCloudEnabled = false;
  
  _arSession = [ARSession new];
  [_arSession setDelegate:_arDelegate];
  
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
  
  ARWorldTrackingConfiguration *configuration = [ARWorldTrackingConfiguration new];
  configuration.planeDetection = _planeDetectionEnabled ? ARPlaneDetectionHorizontal : ARPlaneDetectionNone;
  [_arSession runWithConfiguration: configuration];
  
  return true;
}

mat4 ARKitTracker::getCameraView() {
  UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
  matrix_float4x4 view = matrix_invert([_arSession.currentFrame.camera viewMatrixForOrientation:orientation]);
  return (float*)&view.columns[0];
}

mat4 ARKitTracker::getCameraProjection() {
  CGSize viewport = UIScreen.mainScreen.bounds.size;
  UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
  matrix_float4x4 proj = [_arSession.currentFrame.camera projectionMatrixForOrientation:orientation
                                                    viewportSize:viewport
                                                           zNear:0.001
                                                            zFar:1000.0];
  return (float*)&proj.columns[0];
}

mat4 ARKitTracker::getImageTransform() {
  mat4 transform;
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

void ARKitTracker::enablePointCloud(bool enable) {
  _pointCloudEnabled = enable;
}

const ARPointVector& ARKitTracker::getPointCloud() const {
  return _pointCloud;
}

TextureBufferPtr ARKitTracker::getCameraImageY() {
  return _cameraImageY;
}

TextureBufferPtr ARKitTracker::getCameraImageCbCr() {
  return _cameraImageCbCr;
}

const TrackedPlanes& ARKitTracker::trackedPlanes() const {
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
  
  if (_pointCloudEnabled) {
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
  matrix_float4x4 transform = anchor.transform;
  vector_float3 center = anchor.center;
  
  TrackedPlane plane;
  plane.uuid = [[anchor.identifier UUIDString] UTF8String];
  plane.transform = mat4((float*)&transform.columns[0]);
  plane.center = vec3(center.x, center.y, center.z);
  plane.position = plane.transform.w.xyz();
  plane.rotation = quat(plane.transform);
  plane.extent = vec2(anchor.extent.x, anchor.extent.z);
  
  updateTrackedPlane(plane);
  if (_delegate != nullptr)
    _delegate->trackedPlaneAdded(plane);
}

void ARKitTracker::planeAnchorUpdated(ARPlaneAnchor *anchor) {
  matrix_float4x4 transform = anchor.transform;
  vector_float3 center = anchor.center;
  
  TrackedPlane plane;
  plane.uuid = [[anchor.identifier UUIDString] UTF8String];
  plane.transform = mat4((float*)&transform.columns[0]);
  plane.center = vec3(center.x, center.y, center.z);
  plane.position = plane.transform.w.xyz();
  plane.rotation = quat(plane.transform);
  plane.extent = vec2(anchor.extent.x, anchor.extent.z);
  
  updateTrackedPlane(plane);
  if (_delegate != nullptr)
    _delegate->trackedPlaneUpdated(plane);
}

void ARKitTracker::planeAnchorRemoved(ARPlaneAnchor *anchor) {
  matrix_float4x4 transform = anchor.transform;
  vector_float3 center = anchor.center;
  
  TrackedPlane plane;
  plane.uuid = [[anchor.identifier UUIDString] UTF8String];
  plane.transform = mat4((float*)&transform.columns[0]);
  plane.center = vec3(center.x, center.y, center.z);
  plane.position = plane.transform.w.xyz();
  plane.rotation = quat(plane.transform);
  plane.extent = vec2(anchor.extent.x, anchor.extent.z);
  
  updateTrackedPlane(plane);
  if (_delegate != nullptr)
    _delegate->trackedPlaneRemoved(plane);
}

void ARKitTracker::updateTrackedPlane(const TrackedPlane &plane) {
  for (auto &tracked : _trackedPlanes) {
    if (tracked.uuid == plane.uuid) {
      tracked = plane;
      return;
    }
  }
  _trackedPlanes.push_back(plane);
}

//void ARKitTracker::calculateVertices(vector<vec4> &vertices) {
////  // Add the Vertices
////  vertices.clear();
////  vertices.push_back(vec4(-1.0f, -1.0f, 0.0f, 1.0f));
////  vertices.push_back(vec4(-1.0f,  1.0f, 0.0f, 0.0f));
////  vertices.push_back(vec4(1.0f, -1.0f, 1.0f, 1.0f));
////  vertices.push_back(vec4(1.0f,  1.0f, 1.0f, 0.0f));
////
////  ARFrame *frame = _arSession.currentFrame;
////  if (frame != nil) {
////    // Get the Image Correction Transform
////    CGSize viewport = CGSizeMake(_task.frame->size().x, _task.frame->size().y);
////    CGAffineTransform affine = [frame displayTransformForOrientation:UIInterfaceOrientationPortrait viewportSize:viewport];
////    affine = CGAffineTransformInvert(affine);
////
////    // Transform the UVs of the Vertices
////    for (auto &vertex : vertices) {
////      CGPoint coord = CGPointApplyAffineTransform(CGPointMake(vertex.z, vertex.w), affine);
////      vertex.z = coord.x;
////      vertex.w = coord.y;
////    }
////  }
//}
