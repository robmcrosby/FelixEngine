//
//  ViewController.m
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "XMLScene.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

@interface ViewController ()

@property XMLScene *xmlSceneApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.xmlSceneApp = new XMLScene();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.xmlSceneApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.xmlSceneApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.xmlSceneApp];
  self.xmlSceneApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}


@end
