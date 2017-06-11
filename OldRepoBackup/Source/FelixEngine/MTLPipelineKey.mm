//
//  MTLKey.m
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"
#include "GraphicTask.h"

#if !TARGET_IPHONE_SIMULATOR

using namespace fx;

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
    _blendFlags = 0;
  }
  return self;
}

-(id)copyWithZone:(NSZone *)zone
{
  MTLPipelineKey *key = [[MTLPipelineKey allocWithZone:zone] init];
  
  if (key != nil)
  {
    key.shader = _shader;
    key.blendFlags = _blendFlags;
  }
  
  return key;
}

-(BOOL)isEqual:(id)object
{
  // Check if object is a MTLKey
  if (![object isKindOfClass:[MTLPipelineKey class]])
    return NO;
  
  MTLPipelineKey *other = (MTLPipelineKey*)object;
  
  if (_blendFlags != other.blendFlags)
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

-(BOOL)blendingEnabled
{
  BlendState state((int)_blendFlags);
  return (BOOL)state.enabled();
}

-(BOOL)alphaOperations
{
  BlendState state((int)_blendFlags);
  return (BOOL)state.seperate();
}

-(MTLBlendOperation)getMTLBlendOperation:(int)flag
{
  switch (flag)
  {
    case BLEND_EQ_ADD:          return MTLBlendOperationAdd;
    case BLEND_EQ_SUBTRACT:     return MTLBlendOperationSubtract;
    case BLEND_EQ_REV_SUBTRACT: return MTLBlendOperationReverseSubtract;
    case BLEND_EQ_MIN:          return MTLBlendOperationMin;
    case BLEND_EQ_MAX:          return MTLBlendOperationMax;
  }
  return MTLBlendOperationAdd;
}

-(MTLBlendOperation)colorBlendOperation
{
  BlendState state((int)_blendFlags);
  return [self getMTLBlendOperation:state.equation()];
}

-(MTLBlendOperation)alphaBlendOperation
{
  BlendState state((int)_blendFlags);
  return [self getMTLBlendOperation:state.equationAlpha()];
}

-(MTLBlendFactor)getMTLBlendFactor:(int)flag
{
  if (flag == BLEND_INPUT_SRC_ALPHA_SATURATE)
    return MTLBlendFactorSourceAlphaSaturated;
  if (flag & BLEND_INPUT_ONE_MINUS)
  {
    flag &= ~BLEND_INPUT_ONE_MINUS;
    switch (flag)
    {
      case BLEND_INPUT_SRC_COLOR:   return MTLBlendFactorOneMinusSourceColor;
      case BLEND_INPUT_SRC_ALPHA:   return MTLBlendFactorOneMinusSourceAlpha;
      case BLEND_INPUT_DST_COLOR:   return MTLBlendFactorOneMinusDestinationColor;
      case BLEND_INPUT_DST_ALPHA:   return MTLBlendFactorOneMinusDestinationAlpha;
      case BLEND_INPUT_CONST_COLOR: return MTLBlendFactorOneMinusBlendColor;
      case BLEND_INPUT_CONST_ALPHA: return MTLBlendFactorOneMinusBlendAlpha;
    }
  }
  else
  {
    switch (flag)
    {
      case BLEND_INPUT_SRC_COLOR:   return MTLBlendFactorSourceColor;
      case BLEND_INPUT_SRC_ALPHA:   return MTLBlendFactorSourceAlpha;
      case BLEND_INPUT_DST_COLOR:   return MTLBlendFactorDestinationColor;
      case BLEND_INPUT_DST_ALPHA:   return MTLBlendFactorDestinationAlpha;
      case BLEND_INPUT_CONST_COLOR: return MTLBlendFactorBlendColor;
      case BLEND_INPUT_CONST_ALPHA: return MTLBlendFactorBlendAlpha;
      case BLEND_INPUT_ZERO:        return MTLBlendFactorZero;
    }
  }
  return MTLBlendFactorOne;
}

-(MTLBlendFactor)sourceColorBlendFactor
{
  BlendState state((int)_blendFlags);
  return [self getMTLBlendFactor:state.src()];
}

-(MTLBlendFactor)destColorBlendFactor
{
  BlendState state((int)_blendFlags);
  return [self getMTLBlendFactor:state.dst()];
}

-(MTLBlendFactor)sourceAlphaBlendFactor
{
  BlendState state((int)_blendFlags);
  return [self getMTLBlendFactor:state.srcAlpha()];
}

-(MTLBlendFactor)destAlphaBlendFactor
{
  BlendState state((int)_blendFlags);
  return [self getMTLBlendFactor:state.dstAlpha()];
}

@end

#endif
