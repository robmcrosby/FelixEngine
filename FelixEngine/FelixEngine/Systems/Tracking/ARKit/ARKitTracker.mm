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

@end


using namespace fx;
using namespace std;

ARKitTracker::ARKitTracker() {
  instance = this;
  _graphics = nullptr;
  
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

void ARKitTracker::arSessionFailed() {
  cout << "AR Session Failed With Error" << endl;
}

void ARKitTracker::arSessionInterupted() {
  cout << "AR Session Interupted" << endl;
}

void ARKitTracker::arSessionInteruptEnd() {
  cout << "AR Session Interuption End" << endl;
}
