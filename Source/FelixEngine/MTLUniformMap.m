//
//  MTLUniformMap.m
//  FelixEngine
//
//  Created by Robert Crosby on 10/23/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"


@implementation MTLUniformMap
{
  id <MTLDevice> _mtlDevice;
}

-(id)initWithDevice:(id <MTLDevice>)device
{
  self = [super init];
  
  if (self)
  {
    _mtlDevice = device;
    _uniforms = nil;
    _buffer = nil;
  }
  
  return self;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _uniforms = nil;
  _buffer = nil;
}

-(void)setUniformsSize:(NSUInteger)size
{
  if (_uniforms != nil && [_uniforms count] != size)
    _uniforms = nil;
  if (_uniforms == nil)
  {
    NSMutableArray *uniforms = [NSMutableArray array];
    for (int i = 0; i < size; ++i)
      [uniforms addObject:[[MTLUniform alloc] init]];
    _uniforms = [NSArray arrayWithArray:uniforms];
  }
}

-(void)setBufferSize:(NSUInteger)size
{
  if (_mtlDevice != nil)
  {
    if (_buffer != nil && [_buffer length] < size)
      _buffer = nil;
    if (_buffer == nil)
      _buffer = [_mtlDevice newBufferWithLength:size options:0];
  }
}

-(void)applyToShader:(MTLShader*)shader withEncoder:(id <MTLRenderCommandEncoder>)encoder
{
  if (shader != nil && encoder != nil && _buffer != nil)
  {
    for (MTLUniform *uniform in _uniforms)
    {
      NSInteger index = [shader getVertexLocationOf:uniform.name];
      if (index >= 0)
        [encoder setVertexBuffer:_buffer offset:uniform.offset atIndex:index];
      
      index = [shader getFragmentLocationOf:uniform.name];
      if (index >= 0)
        [encoder setFragmentBuffer:_buffer offset:uniform.offset atIndex:index];
    }
  }
}

@end



@implementation MTLUniform

-(id)init
{
  self = [super init];
  
  if (self != nil)
  {
    _name = @"";
    _offset = 0;
  }
  
  return self;
}

-(id)copyWithZone:(NSZone *)zone
{
  MTLUniform *uniform = [[MTLUniform allocWithZone:zone] init];
  
  if (uniform != nil)
  {
    [uniform setName:_name];
    [uniform setOffset:_offset];
  }
  
  return uniform;
}

@end
