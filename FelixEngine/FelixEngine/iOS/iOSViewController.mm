//
//  iOSViewController.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "iOSViewController.h"
#import "Application.h"
#import <vector>

@interface iOSViewController()

@property(nonatomic) fx::Application *cppApplication;
@property(nonatomic) CADisplayLink *displayLink;
@property(nonatomic) std::vector<id> touchNumbers;

@end


@implementation iOSViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  self.application = nullptr;
  self.displayLink = nil;
  
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillEnterForeground:) name:UIApplicationWillEnterForegroundNotification object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillTerminate:) name:UIApplicationWillTerminateNotification object:nil];
  
  [self.view setMultipleTouchEnabled:YES];
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
}

- (BOOL)prefersStatusBarHidden {
  return YES;
}

- (BOOL)shouldAutorotate {
  return NO;
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

- (int)addTouchNumber:(id)touchId {
  int index = 0;
  for (; index < self.touchNumbers.size(); ++index) {
    if (_touchNumbers.at(index) == nil) {
      _touchNumbers.at(index) = touchId;
      return index;
    }
  }
  _touchNumbers.push_back(touchId);
  return index;
}

- (int)getTouchNumber:(id)touchId {
  int index = 0;
  for (; index < _touchNumbers.size(); ++index) {
    if (_touchNumbers.at(index) == touchId)
      return index;
  }
  return [self addTouchNumber:touchId];
}

- (int)removeTouchNumber:(id)touchId {
  int index = 0;
  for (; index < _touchNumbers.size(); ++index) {
    if (_touchNumbers.at(index) == touchId) {
      _touchNumbers.at(index) = nil;
      return index;
    }
  }
  return index;
}

- (fx::Event)createInputEvent:(fx::INPUT_TYPE)type withTouches:(NSSet<UITouch *> *)touches {
  fx::Touches touchPoints;
  CGSize size = self.view.bounds.size;
  for (UITouch *touch in touches) {
    fx::Touch touchPoint;
    CGPoint location = [touch preciseLocationInView:self.view];
    CGPoint previous = [touch precisePreviousLocationInView:self.view];
    
    // Get touch locations and force
    touchPoint.location = fx::vec2(location.x/size.width, location.y/size.height);
    touchPoint.pevious = fx::vec2(previous.x/size.width, previous.y/size.height);
    touchPoint.force = touch.force/touch.maximumPossibleForce;
    
    // Check if the touch is a stylus
    if (touch.type == UITouchTypeStylus) {
      CGVector azimuth = [touch azimuthUnitVectorInView:nil];
      touchPoint.azimuth = fx::vec2(azimuth.dx, azimuth.dy);
      touchPoint.type = fx::TOUCH_STYLUS;
    }
    
    // Get the Touch Number
    if (type == fx::INPUT_TOUCH_DOWN)
      touchPoint.number = [self addTouchNumber:touch];
    else if (type == fx::INPUT_TOUCH_MOVE)
      touchPoint.number = [self getTouchNumber:touch];
    else
      touchPoint.number = [self removeTouchNumber:touch];
    
    // Add to the touch points
    touchPoints.push_back(touchPoint);
  }
  return fx::Event(type, touchPoints);
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  _cppApplication->handle([self createInputEvent:fx::INPUT_TOUCH_DOWN withTouches:touches]);
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  _cppApplication->handle([self createInputEvent:fx::INPUT_TOUCH_UP withTouches:touches]);
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  _cppApplication->handle([self createInputEvent:fx::INPUT_TOUCH_CANCEL withTouches:touches]);
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  _cppApplication->handle([self createInputEvent:fx::INPUT_TOUCH_MOVE withTouches:touches]);
}



@end
