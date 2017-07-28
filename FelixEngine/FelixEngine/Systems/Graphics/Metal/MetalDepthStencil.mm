//
//  MetalDepthStencil.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/28/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "MetalDepthStencil.h"
#import "GraphicTask.h"


using namespace fx;

@interface MetalDepthStencil()

@property id<MTLDevice> device;
@property NSMutableDictionary *depthStencilStates;

@end

@implementation MetalDepthStencil

-(id)initWithDevice:(id <MTLDevice>)device {
  self = [super init];
  if (self != nil) {
    self.device = device;
    self.depthStencilStates = [[NSMutableDictionary alloc] init];
  }
  return self;
}

- (id <MTLDepthStencilState>) depthStencilStateForFlags:(int)flags {
  NSNumber *key = [NSNumber numberWithInt:flags];
  id <MTLDepthStencilState> depthStencilState = [self.depthStencilStates objectForKey:key];
  
  if (depthStencilState == nil) {
    DepthState state(flags);
    MTLDepthStencilDescriptor *descriptor = [MTLDepthStencilDescriptor new];
    
    descriptor.depthWriteEnabled = state.writingEnabled();
    descriptor.depthCompareFunction = MTLCompareFunctionAlways;
    
    if (state.function() == DEPTH_TEST_LESS)
      descriptor.depthCompareFunction = MTLCompareFunctionLess;
    else if (state.function() == DEPTH_TEST_GREATER)
      descriptor.depthCompareFunction = MTLCompareFunctionGreater;
    else if (state.function() == DEPTH_TEST_EQUAL)
      descriptor.depthCompareFunction = MTLCompareFunctionEqual;
    else if (state.function() == DEPTH_TEST_LESS_EQ)
      descriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
    else if (state.function() == DEPTH_TEST_GREATER_EQ)
      descriptor.depthCompareFunction = MTLCompareFunctionGreaterEqual;
    else if (state.function() == DEPTH_TEST_NEVER)
      descriptor.depthCompareFunction = MTLCompareFunctionNever;
    
    depthStencilState = [self.device newDepthStencilStateWithDescriptor:descriptor];
    [self.depthStencilStates setObject:depthStencilState forKey:key];
  }
  return depthStencilState;
}

@end
