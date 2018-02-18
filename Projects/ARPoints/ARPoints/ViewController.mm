//
//  ViewController.m
//  ARPoints
//
//  Created by Robert Crosby on 2/18/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "ARPoints.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>
#import <FelixEngine/ARKitTracker.h>

@interface ViewController ()

@property ARPoints *arPointsApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.arPointsApp = new ARPoints();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.arPointsApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.arPointsApp->setGraphics(graphics);
  
  // Add and initalize ARKit
  fx::ARKitTracker *tracker = new fx::ARKitTracker();
  tracker->initalize(graphics);
  self.arPointsApp->setTrackerSystem(tracker);
  
  // Set the Application
  [self setApplication: self.arPointsApp];
  self.arPointsApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}


@end
