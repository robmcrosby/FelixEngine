//
//  DetailViewController.m
//  ModelViewer
//
//  Created by Robert Crosby on 12/21/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "DetailViewController.h"
#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>
#import "ModelViewer.h"


@interface DetailViewController ()
@property ModelViewer *modelViewerApp;

@end

@implementation DetailViewController

- (void)viewDidLoad {
    [super viewDidLoad];
  
  // Create the Cpp Application
  self.modelViewerApp = new ModelViewer();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.modelViewerApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  graphics->initalize(self.view);
  self.modelViewerApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.modelViewerApp];
  self.modelViewerApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)showPictureAtUrl:(NSURL*)url {
  [self.splitViewController showDetailViewController:self sender:nil];
  NSLog(@"Show Picture: %@", [url lastPathComponent]);
}


@end
