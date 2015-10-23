//
//  MTLFrameObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//


#import "MTLGraphicResources.h"


@implementation MTLFrame
{
  id <MTLDevice> _mtlDevice;
  
  NSMutableDictionary *_pipelines;
  NSMutableDictionary *_descriptors;
  NSMutableArray *_colorBuffers;
  MTLTexture     *_depthBuffer;
}

-(id)initWithDevice:(id <MTLDevice>)device
{
  self = [super init];
  
  if (self != nil)
  {
    _mtlDevice = device;
    
    _width = 0;
    _height = 0;
    
    _pipelines = [NSMutableDictionary dictionary];
    _colorBuffers = [NSMutableArray array];
    _depthBuffer = nil;
    
    static NSUInteger frameCount;
    _frameId = frameCount++;
  }
  
  return self;
}

-(void)setMetalDevice:(id <MTLDevice>)device
{
  _mtlDevice = device;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _pipelines = nil;
  _colorBuffers = nil;
  _depthBuffer = nil;
}

-(void)resizeToWidth:(NSUInteger)width andHeight:(NSUInteger)height
{
  if (_width != width || _height != height)
  {
    _width = width;
    _height = height;
    
    for (MTLTexture *buffer in _colorBuffers)
      [buffer resizeToWidth:_width andHeight:_height];
    
    if (_depthBuffer != nil)
      [_depthBuffer resizeToWidth:_width andHeight:_height];
    
    [_descriptors removeAllObjects];
  }
}

-(void)setWidth:(NSUInteger)width andHeight:(NSUInteger)height
{
  _width = width;
  _height = height;
}

-(void)updateWindowBuffer
{
//  for (MTLTexture *buffer in _colorBuffers)
//  {
//    _width = buffer.width;
//    _height = buffer.height;
//  }
//  if (_depthBuffer != nil)
//    [_depthBuffer resizeToWidth:_width andHeight:_height];
  
  [_descriptors removeAllObjects];
}

-(id <MTLRenderPipelineState>)getPipelineForKey:(MTLPipelineKey*)key
{
  id <MTLRenderPipelineState> pipeline = [_pipelines objectForKey:key];
  if (pipeline == nil && key.shader != nil && _mtlDevice != nil)
  {
    // Create the pipeline.
    MTLRenderPipelineDescriptor *pipelineDesc = [MTLRenderPipelineDescriptor new];
    
    NSUInteger index = 0;
    for (MTLTexture *buffer in _colorBuffers)
    {
      pipelineDesc.colorAttachments[index].pixelFormat = buffer.format;
      
      if (key.blendingEnabled)
      {
        pipelineDesc.colorAttachments[index].blendingEnabled = YES;
        pipelineDesc.colorAttachments[index].rgbBlendOperation   = MTLBlendOperationAdd;
        //pipelineDesc.colorAttachments[index].alphaBlendOperation = MTLBlendOperationAdd;
        pipelineDesc.colorAttachments[index].sourceRGBBlendFactor   = MTLBlendFactorBlendAlpha;
        //pipelineDesc.colorAttachments[index].sourceAlphaBlendFactor = MTLBlendFactorOne;
        pipelineDesc.colorAttachments[index].destinationRGBBlendFactor   = MTLBlendFactorOneMinusSourceAlpha;
        //pipelineDesc.colorAttachments[index].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
      }
      ++index;
    }
    
    if (_depthBuffer != nil)
      pipelineDesc.depthAttachmentPixelFormat = _depthBuffer.format;
    else
      pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
    
    pipelineDesc.sampleCount      = 1;
    pipelineDesc.vertexFunction   = key.shader.vertexFunction;
    pipelineDesc.fragmentFunction = key.shader.fragmentFunction;
    
    NSError *error = nil;
    
    if (![key.shader hasReflection])
    {
      // Get the reflection data of the shader (Should do this only in the shader.)
      MTLRenderPipelineReflection *reflection;
      pipeline = [_mtlDevice newRenderPipelineStateWithDescriptor:pipelineDesc options:MTLPipelineOptionArgumentInfo reflection:&reflection error:&error];
      
      [key.shader setReflection:reflection];
      reflection = nil;
    }
    else
    {
      // Reflection is not needed if the shader alreay had it done with another pipeline.
      pipeline = [_mtlDevice newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    }
    
    if (pipeline != nil)
      [_pipelines setObject:pipeline forKey:key];
  }
  
  return pipeline;
}

-(MTLRenderPassDescriptor *)getDescriptorForKey:(MTLDescriptorKey*)key
{
  MTLRenderPassDescriptor *descriptor = [_descriptors objectForKey:key];
  if (descriptor == nil)
  {
    descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    
    NSUInteger index = 0;
    for (MTLTexture *buffer in _colorBuffers)
    {
      descriptor.colorAttachments[index].texture = buffer.texture;
      descriptor.colorAttachments[index].storeAction = MTLStoreActionStore;
      
      if (key.clearColorBuffers)
      {
        descriptor.colorAttachments[index].loadAction  = MTLLoadActionClear;
        descriptor.colorAttachments[index].clearColor  = key.clearColor;
      }
      else
        descriptor.colorAttachments[index].loadAction  = MTLLoadActionLoad;
      
      ++index;
    }
    
    if (_depthBuffer != nil)
    {
      descriptor.depthAttachment.texture = _depthBuffer.texture;
      descriptor.depthAttachment.storeAction = MTLStoreActionStore;
      
      if (key.clearDepthBuffer)
      {
        descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        descriptor.depthAttachment.clearDepth = (double)key.clearDepth;
      }
      else
        descriptor.depthAttachment.loadAction = MTLLoadActionLoad;
    }
    
    [_descriptors setObject:descriptor forKey:key];
  }
  
  return descriptor;
}


-(void)addColorBuffer:(MTLTexture*)texture
{
  [_colorBuffers addObject:texture];
  
  // Clear the pipeline and descriptor dictionaries for the changes
  [_pipelines removeAllObjects];
  [_descriptors removeAllObjects];
  
  //[texture resizeToWidth:_width andHeight:_height];
}

-(void)setDepthBuffer:(MTLTexture*)texture
{
  _depthBuffer = texture;
  
  [_pipelines removeAllObjects];
  [_descriptors removeAllObjects];
  
  if (_depthBuffer != nil)
    [_depthBuffer resizeToWidth:_width andHeight:_height];
}


@end
