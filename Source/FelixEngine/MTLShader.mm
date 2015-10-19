//
//  MTLShaderObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"

@implementation MTLShader
{
  id <MTLDevice> _mtlDevice;
  id <MTLLibrary> _mtlLibrary;
  
  MTLRenderPipelineReflection *_reflection;
  NSDictionary *_vertexLocations;
  NSDictionary *_fragmentLocations;
}

-(id)initWithDevice:(id <MTLDevice>)device andLibrary:(id <MTLLibrary>)library
{
  self = [super init];
  
  if (self != nil && device != nil && library != nil)
  {
    _mtlDevice = device;
    _mtlLibrary = library;
    
    _vertexFunction = nil;
    _fragmentFunction = nil;
    
    _reflection = nil;
    _vertexLocations = nil;
    _fragmentFunction = nil;
    
    static NSUInteger shaderCount = 0;
    _shaderId = shaderCount++;
  }
  
  return self;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _mtlLibrary = nil;
  
  _vertexFunction = nil;
  _fragmentFunction = nil;
  
  _reflection = nil;
  _vertexLocations = nil;
  _fragmentFunction = nil;
}

-(BOOL)setVertexFunctionToName:(NSString*)name
{
  _vertexFunction = [_mtlLibrary newFunctionWithName:name];
  return self.vertexFunction != nil;
}

-(BOOL)setFragmentFunctionToName:(NSString*)name
{
  _fragmentFunction = [_mtlLibrary newFunctionWithName:name];
  return self.fragmentFunction!= nil;
}

-(BOOL)hasReflection
{
  return _reflection != nil;
}

-(void)setReflection:(MTLRenderPipelineReflection*)reflection
{
  _reflection = reflection;
  NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
  
  for (MTLArgument *arg in reflection.vertexArguments)
    [dictionary setObject:[NSNumber numberWithInteger:arg.index] forKey:arg.name];
  _vertexLocations = [NSDictionary dictionaryWithDictionary:dictionary];
  
  [dictionary removeAllObjects];
  for (MTLArgument *arg in reflection.fragmentArguments)
    [dictionary setObject:[NSNumber numberWithInteger:arg.index] forKey:arg.name];
  _fragmentLocations = [NSDictionary dictionaryWithDictionary:dictionary];
}

-(NSInteger)getVertexLocationOf:(NSString*)name
{
  NSInteger location = -1;
  
  if (_vertexLocations != nil)
  {
    NSNumber *number = [_vertexLocations objectForKey:name];
    if (number != nil)
      location = [number integerValue];
  }
  
  return location;
}

-(NSInteger)getFragmentLocationOf:(NSString*)name
{
  NSInteger location = -1;
  
  if (_fragmentLocations != nil)
  {
    NSNumber *number = [_fragmentLocations objectForKey:name];
    if (number != nil)
      location = [number integerValue];
  }
  
  return location;
}

-(NSInteger)getAttributeLocationOf:(NSString*)name
{
  return [self getVertexLocationOf:name];
}

@end
