//
//  iOSViewController.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "iOSViewController.h"
#import "Application.h"

@interface iOSViewController ()

@property fx::Application *cppApplication;
@property CADisplayLink *displayLink;

@end


@implementation iOSViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  self.application = nullptr;
  self.displayLink = nil;
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}

-(void)setApplication:(void*)app {
  self.cppApplication = (fx::Application*)app;
}

- (void)setupDisplayLink {
  self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(frameUpdate:)];
  [self.displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)frameUpdate:(CADisplayLink*)displayLink {
  if (self.cppApplication != nullptr) {
    self.cppApplication->update();
  }
}

@end
