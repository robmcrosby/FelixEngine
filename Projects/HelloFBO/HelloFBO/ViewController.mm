//
//  ViewController.m
//  HelloFBO
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "HelloFBO.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>
//#import <FelixEngine/VulkanGraphics.h>

@interface ViewController ()

@property HelloFBO *helloFBOApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.helloFBOApp = new HelloFBO();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.helloFBOApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  //fx::VulkanGraphics *graphics = new fx::VulkanGraphics();
  graphics->initalize(self.view);
  self.helloFBOApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.helloFBOApp];
  self.helloFBOApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}


- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}


@end

