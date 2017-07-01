//
//  ViewController.m
//  HelloTriangle
//
//  Created by Robert Crosby on 6/11/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "HelloTriangle.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

#import <QuartzCore/CADisplayLink.h>

@interface ViewController ()

@property HelloTriangle *helloTriApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.helloTriApp = new HelloTriangle();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.helloTriApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.helloTriApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.helloTriApp];
  self.helloTriApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}


@end
