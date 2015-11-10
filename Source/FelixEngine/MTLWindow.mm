//
//  MTLWindowObj.m
//  FelixEngine
//
//  Created by Robert Crosby on 9/27/14.
//  Copyright (c) 2014 Robert Crosby. All rights reserved.
//

#import "MTLGraphicResources.h"

#if !TARGET_IPHONE_SIMULATOR


@implementation MTLWindow
{
  id <MTLDevice> _mtlDevice;
  
  MTLTexture *_colorTexture;
  MTLTexture *_depthTexture;
  MTLFrame   *_mtlFrame;

  CAMetalLayer *_layer;
  id <CAMetalDrawable>  _drawable;
  
  #if TARGET_OS_IPHONE
  UIWindow *_uiWindow;
  #else
  NSWindow *_nsWindow;
  #endif
}

-(id)init
{
  self = [super init];
  
  if (self != nil)
  {
    _mtlDevice = nil;
    _colorTexture = nil;
    _depthTexture = nil;
    _mtlFrame = nil;
    _layer = nil;
    _drawable = nil;
  }
  
  return self;
}

-(void)setMetalDevice:(id <MTLDevice>)device
{
  _mtlDevice = device;
  [_mtlFrame setMetalDevice:device];
  
  _colorTexture = [[MTLTexture alloc] initWithDevice:_mtlDevice];
  [_colorTexture setFormat:MTLPixelFormatBGRA8Unorm];
  [_mtlFrame addColorBuffer:_colorTexture];
  
  //_depthTexture = [[MTLTexture alloc] initWithDevice:_mtlDevice];
  //[_depthTexture setFormat:MTLPixelFormatDepth32Float];
  //[_mtlFrame setDepthBuffer:_depthTexture];
}

-(void)setMetalFrame:(MTLFrame*)frame
{
  _mtlFrame = frame;
}

-(void)dealloc
{
  _mtlDevice = nil;
  _colorTexture = nil;
  _depthTexture = nil;
  _mtlFrame = nil;
  _layer = nil;
  _drawable = nil;
}

#if TARGET_OS_IPHONE
-(BOOL)setUIView:(UIView*)uiView
{
  return NO;
}
#else
-(BOOL)setNSWindow:(NSWindow *)window
{
  _nsWindow = window;
  if (_mtlDevice != nil)
  {
    [_nsWindow.contentView setWantsLayer:YES];
    _layer = [CAMetalLayer layer];
    [_nsWindow.contentView setLayer:_layer];
    
    _layer.device = _mtlDevice;
    _layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    return YES;
  }
  return NO;
}
#endif

-(void)setNextDrawable
{
  _drawable = [_layer nextDrawable];
  NSUInteger width = [[_drawable texture] width];
  NSUInteger height = [[_drawable texture] height];
  
  [_colorTexture setTexture:_drawable.texture Format:MTLPixelFormatBGRA8Unorm];
  [_mtlFrame updateWindowBufferAtWidth:width Height:height];
}

-(void)presentDrawable:(id <MTLCommandBuffer>)cmdBuffer
{
  [cmdBuffer presentDrawable:_drawable];
  _drawable = nil;
  [_colorTexture setTexture:nil Format:MTLPixelFormatBGRA8Unorm];
}

@end

#endif
