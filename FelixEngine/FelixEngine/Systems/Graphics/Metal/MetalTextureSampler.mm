//
//  MetalTextureSampler.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "MetalTextureSampler.h"
#import "GraphicTask.h"


using namespace fx;

@interface MetalTextureSampler()

@property id<MTLDevice> device;
@property NSMutableDictionary *samplerStates;

@end

@implementation MetalTextureSampler

-(id)initWithDevice:(id <MTLDevice>)device {
  self = [super init];
  if (self != nil) {
    self.device = device;
    self.samplerStates = [[NSMutableDictionary alloc] init];
  }
  return self;
}

- (id <MTLSamplerState>) samplerStateForFlags:(int)flags {
  NSNumber *key = [NSNumber numberWithInt:flags];
  id <MTLSamplerState> samplerState = [self.samplerStates objectForKey:key];
  
  if (samplerState == nil) {
    SamplerState state(flags);
    MTLSamplerDescriptor *descriptor = [MTLSamplerDescriptor new];
    
    descriptor.minFilter = state.minFilter() == FILTER_LINEAR ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    descriptor.magFilter = state.magFilter() == FILTER_LINEAR ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    
    samplerState = [self.device newSamplerStateWithDescriptor:descriptor];
    [self.samplerStates setObject:samplerState forKey:key];
  }
  return samplerState;
}

@end
