//
//  MTLTextureObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"


@implementation MTLTexture
{
  id <MTLDevice> _mtlDevice;
}

-(id)initWithDevice:(id <MTLDevice>)device;
{
  self = [super init];
  
  if (self != nil && device != nil)
  {
    _mtlDevice = device;
    _texture = nil;
    
    _type   = MTLTextureType2D;
    _format = MTLPixelFormatInvalid;
    _width  = 0;
    _height = 0;
  }
  
  return self;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _texture = nil;
}

-(BOOL)loadImage2d:(const void*)image withWidth:(NSUInteger)width andHeight:(NSUInteger)height
{
  MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
  _type = texDesc.textureType;
  _texture = [_mtlDevice newTextureWithDescriptor:texDesc];
  texDesc = nil;
  
  if (_texture != nil)
  {
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    NSUInteger bytesPerRow = width * 4;
    
    [self.texture replaceRegion:region mipmapLevel:0 withBytes:image bytesPerRow:bytesPerRow];
    
    _format = MTLPixelFormatRGBA8Unorm;
    _width  = width;
    _height = height;
    
    return true;
  }
  
  _format = MTLPixelFormatInvalid;
  _width  = 0;
  _height = 0;
  
  return NO;
}

-(BOOL)resizeToWidth:(NSUInteger)width andHeight:(NSUInteger)height
{
  if (_width == width && _height == height)
    return YES;
  
  if (width > 0 && height > 0)
  {
    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:_format width:width height:height mipmapped:NO];
    _type = texDesc.textureType;
    _texture = [_mtlDevice newTextureWithDescriptor:texDesc];
    texDesc = nil;
    
    if (_texture != nil)
    {
      _width  = width;
      _height = height;
      return YES;
    }
  }
  else
  {
    _width = width;
    _height = height;
    _texture = nil;
    return YES;
  }
  
  _width  = 0;
  _height = 0;
  return NO;
}

-(BOOL)resizeToWidth:(NSUInteger)width andHeight:(NSUInteger)height withFormat:(MTLPixelFormat)format
{
  _format = format;
  _width = _height = 0;
  return [self resizeToWidth:width andHeight:height];
}

-(BOOL)setFormat:(MTLPixelFormat)format
{
  if (_format != format)
    return [self resizeToWidth:_width andHeight:_height withFormat:format];
  return YES;
}

-(void)setTexture:(id<MTLTexture>)texture withFormat:(MTLPixelFormat)format;
{
  _texture = texture;
  
  if (_texture != nil)
  {
    _type = MTLTextureType2D;
    _format = format;
    _width = [_texture width];
    _height = [_texture height];
  }
}

-(void)setToEncoder:(id <MTLRenderCommandEncoder>)encoder atIndex:(NSUInteger)index
{
  [encoder setFragmentTexture:_texture atIndex:index];
}

@end
