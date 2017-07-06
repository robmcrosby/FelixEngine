//
//  ViewController.m
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "ViewController.h"
#import "ExampleScene.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

@interface ViewController ()

@property ExampleScene *exampleSceneApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.exampleSceneApp = new ExampleScene();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.exampleSceneApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.exampleSceneApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.exampleSceneApp];
  self.exampleSceneApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}


@end
