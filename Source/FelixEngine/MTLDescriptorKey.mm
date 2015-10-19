//
//  MTLDescriptorKey.m
//  FelixEngine
//
//  Created by Robert Crosby on 11/26/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"

@implementation MTLDescriptorKey

/**
 * Compares two the color's components and returns true if all are the same or false otherwise.
 */
-(BOOL)compareColor:(MTLClearColor)lhs toColor:(MTLClearColor)rhs
{
  return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue && lhs.alpha == rhs.alpha;
}

/**
 * Encodes the components of a color to a 32 bit intenger value.
 * Assumes the color compoents are normalized between 0.0f to 1.0f.
 */
-(NSUInteger)encodeColor:(MTLClearColor)color
{
  NSUInteger value = _clearColor.red * 255;
  value |= (NSUInteger)(_clearColor.green * 255) << 8;
  value |= (NSUInteger)(_clearColor.blue * 255) << 16;
  value |= (NSUInteger)(_clearColor.alpha * 255) << 24;
  return value;
}

-(id)init
{
  self = [super init];
  
  if (self != nil)
  {
    _clearColor = MTLClearColorMake(0.0f, 0.0f, 0.0f, 1.0f);
    _clearColorBuffers = FALSE;
    
    _clearDepth = 1.0f;
    _clearDepthBuffer = FALSE;
  }
  
  return self;
}

-(id)copyWithZone:(NSZone *)zone
{
  MTLDescriptorKey *key = [[MTLDescriptorKey allocWithZone:zone] init];
  
  if (key != nil)
  {
    [key setClearColor:_clearColor];
    [key setClearColorBuffers:_clearColorBuffers];
    
    [key setClearDepth:_clearDepth];
    [key setClearDepthBuffer:_clearDepthBuffer];
  }
  
  return key;
}

-(BOOL)isEqual:(id)object
{
  // Check if object is a MTLKey
  if (![object isKindOfClass:[MTLDescriptorKey class]])
    return NO;
  
  MTLDescriptorKey *other = (MTLDescriptorKey*)object;
  
  if (_clearColorBuffers != other.clearColorBuffers)
    return NO;
  
  if (_clearColorBuffers && ![self compareColor:_clearColor toColor:other.clearColor])
    return NO;
  
  if (_clearDepthBuffer != other.clearDepthBuffer)
    return NO;
  
  if (_clearDepthBuffer && _clearDepth != other.clearDepth)
    return NO;
  
  return YES;
}

-(NSUInteger)hash
{
  return [self encodeColor:_clearColor];
}

@end
