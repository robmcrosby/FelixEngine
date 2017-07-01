//
//  ViewController.m
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "HelloTexture.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

@interface ViewController ()

@property HelloTexture *helloTexApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.helloTexApp = new HelloTexture();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.helloTexApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.helloTexApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.helloTexApp];
  self.helloTexApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}


- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}


@end
