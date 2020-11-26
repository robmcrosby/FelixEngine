//
//  ViewController.m
//  CubeMap
//
//  Created by Robert Crosby on 11/25/20.
//

#import "ViewController.h"
#import "CubeMap.h"

#import <FelixEngine/iOSFileSystem.h>
#import <FelixEngine/MetalGraphics.h>

@interface ViewController ()

@property CubeMap *cubeMapApp;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  // Create the Cpp Application
  self.cubeMapApp = new CubeMap();
  
  // Add iOS File System
  fx::iOSFileSystem *fileSystem = new fx::iOSFileSystem();
  self.cubeMapApp->setFileSystem(fileSystem);
  
  // Add and initalize Metal Graphics
  fx::MetalGraphics *graphics = new fx::MetalGraphics();
  //fx::VulkanGraphics *graphics = new fx::VulkanGraphics();
  graphics->initalize(self.view);
  self.cubeMapApp->setGraphics(graphics);
  
  // Set the Application
  [self setApplication: self.cubeMapApp];
  self.cubeMapApp->initalize();
  
  // Start Display Link
  [self setupDisplayLink];
}


@end
