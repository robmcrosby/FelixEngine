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
-(BOOL)setUIWindow:(UIWindow *)window
{
  UIView *uiView = window.rootViewController.view;
  if (_mtlDevice != nil)
  {
    // Setup metal layer and add as sub layer to view
    _layer = [CAMetalLayer layer];
    _layer.device = _mtlDevice;
    _layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    _layer.presentsWithTransaction = NO;
    _layer.drawsAsynchronously     = YES;
    _layer.needsDisplayOnBoundsChange = YES;
    _layer.contentsScale = [UIScreen mainScreen].scale;
    
    // Change this to NO if the compute encoder is used as the last pass on the drawable texture
    _layer.framebufferOnly = YES;
    
    // Add metal layer to the views layer hierarchy
    [_layer setFrame:uiView.layer.frame];
    [uiView.layer addSublayer:_layer];
    
    uiView.opaque = YES;
    uiView.backgroundColor = nil;
    uiView.contentScaleFactor = [UIScreen mainScreen].scale;
    
    if (_mtlFrame != nil)
      [_mtlFrame setWidth:uiView.layer.frame.size.width Height:uiView.layer.frame.size.height];
    
    // Hide the Status Bar
    [UIApplication sharedApplication].statusBarHidden = YES;
    
    return YES;
  }
  
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

-(void)updateSize:(CGSize)size andScale:(CGFloat)scale
{
  if (_layer != nil && (_layer.frame.size.width != size.width || _layer.frame.size.height != size.height))
  {
    [_layer setFrame:CGRectMake(0, 0, size.width, size.height)];
    [_layer setDrawableSize:CGSizeMake(size.width*scale, size.height*scale)];
  }
}

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
