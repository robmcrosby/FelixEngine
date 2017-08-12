//
//  ViewController.m
//  ARPlaneDetect
//
//  Created by Robert Crosby on 8/9/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "ARPlaneDetect.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>
#import <FelixEngine/ARKitTracker.h>

@interface ViewController ()

@property ARPlaneDetect *arTrackerApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.arTrackerApp = new ARPlaneDetect();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.arTrackerApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.arTrackerApp->setGraphics(graphics);
  
  // Add and initalize ARKit
  fx::ARKitTracker *tracker = new fx::ARKitTracker();
  tracker->enablePlaneDetection();
  tracker->setDelegate(self.arTrackerApp);
  tracker->initalize(graphics);
  self.arTrackerApp->setTrackerSystem(tracker);
  
  // Set the Application
  [self setApplication: self.arTrackerApp];
  self.arTrackerApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}

@end
