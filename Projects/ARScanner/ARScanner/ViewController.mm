//
//  ViewController.m
//  ARScanner
//
//  Created by Robert Crosby on 9/28/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "ARScanner.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

@interface ViewController ()

@property ARScanner *arScannerApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.arScannerApp = new ARScanner();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.arScannerApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.arScannerApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.arScannerApp];
  self.arScannerApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}


@end
