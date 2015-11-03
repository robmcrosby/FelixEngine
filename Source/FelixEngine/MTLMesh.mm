//
//  MTLMeshObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"


@implementation MTLMesh
{
  NSMutableDictionary *_vertexBuffers;
  id <MTLDevice> _mtlDevice;
  id <MTLBuffer> _indexBuffer;
  
  NSUInteger _numElements;
  NSUInteger _numIndices;
  
  MTLPrimitiveType _primitiveType;
}

-(id)initWithDevice:(id <MTLDevice>)device;
{
  self = [super init];
  
  if (self != nil && device != nil)
  {
    _mtlDevice = device;
    _vertexBuffers = [NSMutableDictionary dictionary];
    _indexBuffer = nil;
    
    _numElements = 0;
    _numIndices = 0;
    
    _primitiveType = MTLPrimitiveTypeTriangle;
  }
  
  return self;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _vertexBuffers = nil;
  _indexBuffer = nil;
}

-(BOOL)loaded
{
  return [_vertexBuffers count] > 0;
}

-(BOOL)addVertexBuffer:(const void*)buffer withElementSize:(NSUInteger)elementSize withNumElements:(NSUInteger)numElements forName:(NSString*)name
{
  NSUInteger length = elementSize * numElements * sizeof(float);
  id <MTLBuffer> vertexBuffer = [_mtlDevice newBufferWithBytes:(const void*)buffer length:length options:MTLResourceOptionCPUCacheModeDefault];
  
  if (vertexBuffer != nil)
  {
    [_vertexBuffers setObject:vertexBuffer forKey:name];
    _numElements = numElements;
    return YES;
  }
  
  return NO;
}

-(BOOL)setIndexBuffer:(const void*)buffer withNumIndices:(NSUInteger)numIndices
{
  NSUInteger length = numIndices * sizeof(int);
  
  _indexBuffer = [_mtlDevice newBufferWithBytes:buffer length:length options:MTLResourceOptionCPUCacheModeDefault];
  if (_indexBuffer != nil)
  {
    _numIndices = numIndices;
    return YES;
  }
  
  return NO;
}

-(void)setPrimitiveType:(MTLPrimitiveType)type
{
  _primitiveType = type;
}

-(void)drawToEncoder:(id <MTLRenderCommandEncoder>)encoder withShader:(MTLShader*)shader instanceCount:(NSUInteger)instances
{
  if (encoder != nil && shader != nil)
  {
    for (NSString *name in _vertexBuffers)
    {
      NSInteger index = [shader getAttributeLocationOf:name];
      if (index >= 0)
        [encoder setVertexBuffer:_vertexBuffers[name] offset:0 atIndex:index];
    }
    
    if (_indexBuffer != nil)
      [encoder drawIndexedPrimitives:_primitiveType indexCount:_numIndices indexType:MTLIndexTypeUInt32 indexBuffer:_indexBuffer indexBufferOffset:0 instanceCount:instances];
    else
      [encoder drawPrimitives:_primitiveType vertexStart:0 vertexCount:_numElements instanceCount:instances];
  }

}

@end
