//
//  iOSViewController.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "iOSViewController.h"
#import "Application.h"

@interface iOSViewController()

@property fx::Application *cppApplication;
@property CADisplayLink *displayLink;

@end


@implementation iOSViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  self.application = nullptr;
  self.displayLink = nil;
  
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillEnterForeground:) name:UIApplicationWillEnterForegroundNotification object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillTerminate:) name:UIApplicationWillTerminateNotification object:nil];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}

- (BOOL)prefersStatusBarHidden {
  return YES;
}

- (void)setApplication:(void*)app {
  self.cppApplication = (fx::Application*)app;
}

- (void)setupDisplayLink {
  self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(frameUpdate:)];
  [self.displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)frameUpdate:(CADisplayLink*)displayLink {
  if (self.cppApplication != nullptr) {
    self.cppApplication->processFrame();
  }
}

- (void)appWillResignActive:(NSNotification*)note {
  [self.displayLink setPaused:YES];
  self.cppApplication->willEnterBackground();
}

- (void)appWillEnterForeground:(NSNotification*)note {
  self.cppApplication->willEnterForeground();
  [self.displayLink setPaused:NO];
}

- (void)appWillTerminate:(NSNotification*)note {
  self.cppApplication->willTerminate();
  [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillResignActiveNotification object:nil];
  [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillEnterForegroundNotification object:nil];
  [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillTerminateNotification object:nil];
  
}


@end
