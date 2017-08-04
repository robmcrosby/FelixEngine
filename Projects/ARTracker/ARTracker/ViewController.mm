//
//  ViewController.m
//  ARTracker
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "ARTracker.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

@interface ViewController ()

@property ARTracker *arTrackerApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.arTrackerApp = new ARTracker();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.arTrackerApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.arTrackerApp->setGraphics(graphics);
  
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
