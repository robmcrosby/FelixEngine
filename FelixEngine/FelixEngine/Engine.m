//
//  Engine.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/11/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "Engine.h"

@implementation Engine

+ (instancetype)instance {
  static Engine *sharedEngine = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedEngine = [[[self class] alloc] init];
  });
  return sharedEngine;
}

- (void)test {
  NSLog(@"It Works!");
}

@end
