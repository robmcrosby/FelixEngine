//
//  MTLKey.m
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"

@implementation MTLPipelineKey


-(BOOL)compareColor:(MTLClearColor)lhs toColor:(MTLClearColor)rhs
{
  return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue && lhs.alpha == rhs.alpha;
}

-(id)init
{
  self = [super init];
  
  if (self != nil)
  {
    _shader = nil;
    _blendingEnabled = FALSE;
  }
  
  return self;
}

-(id)copyWithZone:(NSZone *)zone
{
  MTLPipelineKey *key = [[MTLPipelineKey allocWithZone:zone] init];
  
  if (key != nil)
  {
    key.shader = _shader;
    key.blendingEnabled = _blendingEnabled;
  }
  
  return key;
}

-(BOOL)isEqual:(id)object
{
  // Check if object is a MTLKey
  if (![object isKindOfClass:[MTLPipelineKey class]])
    return NO;
  
  MTLPipelineKey *other = (MTLPipelineKey*)object;
  
  // Check if blending is the same
  if (_blendingEnabled != other.blendingEnabled)
    return NO;
  
  // Check for nil in both shaders.
  if (_shader == nil || other.shader == nil)
    return _shader == other.shader;
  
  // Check if both shaders are the same.
  return _shader.shaderId == other.shader.shaderId;
}

-(NSUInteger)hash
{
  return [_shader shaderId];
}

@end
