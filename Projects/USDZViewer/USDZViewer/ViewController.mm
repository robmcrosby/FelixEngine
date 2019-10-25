//
//  ViewController.m
//  USDZViewer
//
//  Created by Robert Crosby on 10/22/19.
//  Copyright © 2019 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "UsdzViewer.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>


@interface ViewController ()

@property UsdzViewer *usdzViewer;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.usdzViewer = new UsdzViewer();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.usdzViewer->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  //fx::VulkanGraphics *graphics = new fx::VulkanGraphics();
  graphics->initalize(self.view);
  self.usdzViewer->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.usdzViewer];
  self.usdzViewer->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}


@end
