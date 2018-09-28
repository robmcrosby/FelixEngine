//
//  MetalTextureSampler.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#import "MetalTextureSampler.h"
#import "GraphicStates.h"


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

- (id <MTLSamplerState>)samplerStateForFlags:(int)flags {
  NSNumber *key = [NSNumber numberWithInt:flags];
  id <MTLSamplerState> samplerState = [self.samplerStates objectForKey:key];
  
  if (samplerState == nil) {
    SamplerState state(flags);
    MTLSamplerDescriptor *descriptor = [MTLSamplerDescriptor new];
    
    // Set Min and Mag filters
    descriptor.minFilter = [self getMinMagFilter:state.minFilter()];
    descriptor.magFilter = [self getMinMagFilter:state.magFilter()];
    
    // Set Mip Mapping Filter.
    descriptor.mipFilter = [self getMipFilter:state];
    
    // Set Normalization of Coordinates
    descriptor.normalizedCoordinates = state.isNormalized();
    
    // Set the Samples Per-Sampling
    descriptor.maxAnisotropy = state.samples();
    
    // Set Address Modes
    descriptor.sAddressMode = [self getAddressMode:state.sCoord()];
    descriptor.tAddressMode = [self getAddressMode:state.tCoord()];
    descriptor.rAddressMode = [self getAddressMode:state.rCoord()];
    
    // Create the Metal Sampler State
    samplerState = [self.device newSamplerStateWithDescriptor:descriptor];
    [self.samplerStates setObject:samplerState forKey:key];
  }
  return samplerState;
}

- (MTLSamplerMipFilter)getMipFilter:(SamplerState)state {
  if (state.mipMappingEnabled()) {
    switch (state.mipFilter()) {
      case FILTER_LINEAR:
        return MTLSamplerMipFilterLinear;
      case FILTER_NEAREST:
        return MTLSamplerMipFilterNearest;
    }
  }
  return MTLSamplerMipFilterNotMipmapped;
}

- (MTLSamplerMinMagFilter)getMinMagFilter:(FILTER_TYPE)filter {
  switch (filter) {
    case FILTER_LINEAR:
      return MTLSamplerMinMagFilterLinear;
    case FILTER_NEAREST:
      return MTLSamplerMinMagFilterNearest;
  }
}

- (MTLSamplerAddressMode)getAddressMode:(COORD_TYPE)coord {
  switch (coord) {
    case COORD_CLAMP_EDGE:
      return MTLSamplerAddressModeClampToEdge;
    case COORD_REPEAT:
      return MTLSamplerAddressModeRepeat;
    case COORD_MIRROR:
      return MTLSamplerAddressModeMirrorRepeat;
    case COORD_CLAMP_ZERO:
      return MTLSamplerAddressModeClampToZero;
  }
}


@end
