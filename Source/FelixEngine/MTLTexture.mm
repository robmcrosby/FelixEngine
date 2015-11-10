//
//  MTLTextureObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"

#if !TARGET_IPHONE_SIMULATOR


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
    _usage  = MTLTextureUsageShaderRead;
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

-(BOOL)loaded
{
  return _texture != nil;
}

-(BOOL)loadImage:(const void*)image Width:(NSUInteger)width Height:(NSUInteger)height
{
  MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
  [texDesc setUsage:_usage];
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

-(BOOL)resizeToWidth:(NSUInteger)width Height:(NSUInteger)height
{
  if (_width == width && _height == height)
    return YES;
  
  if (width > 0 && height > 0)
  {
    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:_format width:width height:height mipmapped:NO];
    [texDesc setResourceOptions:MTLResourceStorageModePrivate];
    [texDesc setUsage:_usage];
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

-(BOOL)resizeToWidth:(NSUInteger)width Height:(NSUInteger)height Format:(MTLPixelFormat)format
{
  _format = format;
  _width = _height = 0;
  return [self resizeToWidth:width Height:height];
}

-(BOOL)setFormat:(MTLPixelFormat)format
{
  if (_format != format)
    return [self resizeToWidth:_width Height:_height Format:format];
  return YES;
}

-(void)setTexture:(id<MTLTexture>)texture Format:(MTLPixelFormat)format;
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

-(void)applyToEncoder:(id <MTLRenderCommandEncoder>)encoder atIndex:(NSUInteger)index
{
  [encoder setFragmentTexture:_texture atIndex:index];
}

@end

#endif
